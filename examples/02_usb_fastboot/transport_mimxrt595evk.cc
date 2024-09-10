extern "C" {
// clang-format off
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"
#include "usb_device_descriptor.h"
#include "virtual_com.h"
// clang-format on
}
#include "transport_mimxrt595evk.h"

#include <cstring>

#include "fastboot.h"
#include "pw_allocator/allocator.h"
#include "pw_bytes/span.h"
#include "pw_hex_dump/hex_dump.h"
#include "pw_malloc/malloc.h"
#include "pw_status/status.h"

/* ISR queue size, in packets
 * There are two separate queues - for inbound and outbound fastboot
 * packets. This defines the size of each one individually.
 */
static constexpr size_t ISR_PACKET_QUEUE_SIZE = 16;

/* Enable additional debug output (inbound/outbound packets and
 * queueing state).
 */
#define FASTBOOT_DEBUG 1

#if defined(FASTBOOT_DEBUG) && (FASTBOOT_DEBUG > 0)
#define PW_LOG_LEVEL PW_LOG_LEVEL_DEBUG
#else
#define PW_LOG_LEVEL PW_LOG_LEVEL_INFO
#endif
#include "pw_log/log.h"

/* Fastboot packet structure
 *
 * This can be an incoming or outgoing packet.
 */
struct Packet {
  size_t size;
  uint8_t data[];

  constexpr Packet(size_t size_) : size(size_) {}

  /* Convert from a pointer to the data buffer to the Packet itself
   */
  static inline Packet* from_data_pointer(uint8_t* data) {
    const size_t back_offset =
        offsetof(Packet, data) - (offsetof(Packet, size));
    return reinterpret_cast<Packet*>((uintptr_t)data - back_offset);
  }
};

class PacketAllocator {
 public:
  PacketAllocator(pw::Allocator& alloc) : m_alloc(alloc) {}

  /* Make a Packet object for the specified data buffer,
   */
  Packet* MakePacket(uint8_t const* buf, size_t len) {
    taskENTER_CRITICAL();
    auto* packet = AllocAndCopyData(buf, len);
    taskEXIT_CRITICAL();
    return packet;
  }

  /* Make a Packet object for the specified data buffer, ISR-safe
   */
  Packet* MakePacketFromISR(uint8_t const* buf, size_t len) {
    auto saved = taskENTER_CRITICAL_FROM_ISR();
    auto* packet = AllocAndCopyData(buf, len);
    taskEXIT_CRITICAL_FROM_ISR(saved);
    return packet;
  }

  /* Free a given Packet object
   */
  void Free(Packet* ptr) {
    taskENTER_CRITICAL();
    m_alloc.Deallocate(ptr);
    taskEXIT_CRITICAL();
  }

  /* Free a given Packet object, ISR-safe
   */
  void FreeFromISR(Packet* ptr) {
    auto saved = taskENTER_CRITICAL_FROM_ISR();
    m_alloc.Deallocate(ptr);
    taskEXIT_CRITICAL_FROM_ISR(saved);
  }

 private:
  Packet* AllocAndCopyData(uint8_t const* buf, size_t len) {
    auto* packet = Alloc(len);
    if (!packet) {
      return nullptr;
    }
    std::memcpy(packet->data, buf, len);
    return packet;
  }

  Packet* Alloc(size_t len) {
    auto* buf = m_alloc.Allocate(
        pw::allocator::Layout(sizeof(Packet) + len, alignof(Packet)));
    if (!buf) {
      return nullptr;
    }

    return new (buf) Packet(len);
  }

  pw::Allocator& m_alloc;
};

/* Wrapper for storing pointers to objects in a FreeRTOS queue
 */
template <typename T>
class PointerQueue {
 public:
  PointerQueue(size_t queue_size) {
    m_queue = xQueueCreate(queue_size, sizeof(T*));
  }

  /* Enqueue a pointer from a task, will block until space is available
   * Returns true on success
   */
  bool Queue(T* ptr) {
    if (!m_queue) {
      return false;
    }
    const auto err = xQueueSendToBack(m_queue, &ptr, portMAX_DELAY);
    return err == pdTRUE;
  }

  /* Enqueue a pointer from an ISR, won't block if no space is available
   * Returns true on success
   */
  bool QueueFromISR(T* ptr) {
    if (!m_queue) {
      return false;
    }
    const auto err = xQueueSendToBackFromISR(m_queue, &ptr, 0);
    return err == pdTRUE;
  }

  /* Dequeue a pointer from a task, will block until data is available
   * Returns pointer, or nullptr if no data is available.
   */
  T* Dequeue() {
    if (!m_queue) {
      return nullptr;
    }
    T* ptr = nullptr;
    const auto err = xQueueReceive(m_queue, &ptr, portMAX_DELAY);
    return (err == pdTRUE ? ptr : nullptr);
  }

  /* Dequeue a pointer from an ISR, won't block if no data is available
   * Returns pointer, or nullptr if no data is available.
   */
  T* DequeueFromISR() {
    if (!m_queue) {
      return nullptr;
    }
    T* ptr = nullptr;
    const auto err = xQueueReceiveFromISR(m_queue, &ptr, 0);
    return (err == pdTRUE ? ptr : nullptr);
  }

 private:
  QueueHandle_t m_queue;
};

static PacketAllocator s_packet_allocator{*pw::malloc::GetSystemAllocator()};
static PointerQueue<Packet> s_packets_in{ISR_PACKET_QUEUE_SIZE};
static PointerQueue<Packet> s_packets_out{ISR_PACKET_QUEUE_SIZE};

static void HexdumpWithAscii(uint8_t const* buf, size_t len) {
  constexpr pw::dump::FormattedHexDumper::Flags flags = {
      .bytes_per_line = 16,
      .group_every = 1,
      .show_ascii = true,
      .show_header = false,
      .prefix_mode = pw::dump::FormattedHexDumper::AddressMode::kAbsolute};
  // Stack-based buffer
  char linebuffer[128];

  auto dumper = pw::dump::FormattedHexDumper(
      std::span{linebuffer, sizeof(linebuffer)}, flags);
  if (dumper.BeginDump(pw::ConstByteSpan{
          reinterpret_cast<std::byte const*>(buf), len}) != PW_STATUS_OK) {
    return;
  }

  while (dumper.DumpLine() == PW_STATUS_OK) {
    PW_LOG_DEBUG("%s", linebuffer);
  }
}

// Initialize the USB transport
static void UsbInit() {
  // NOTE: This is separated from fastboot::mimxrt595evk::UsbTransportInit
  // because the extern would point to `fastboot::mimxrt595evk::s_cdcVcom`
  // instead of the intended global symbol `s_cdcVcom` from virtual_com.c
  extern usb_cdc_vcom_struct_t s_cdcVcom;
  APPTask(&s_cdcVcom);
}

/* Try enqueueing a fastboot packet
 */
static usb_status_t UsbEnqueueFastbootPacket(Packet* packet) {
  extern usb_cdc_vcom_struct_t s_cdcVcom;
  const auto error = USB_DeviceCdcAcmSend(s_cdcVcom.cdcAcmHandle,
                                          FASTBOOT_USB_BULK_IN_ENDPOINT,
                                          (uint8_t*)packet->data,
                                          packet->size);
  if (error != kStatus_USB_Success && error != kStatus_USB_Busy) {
    // PW_LOG_ERROR("UsbEnqueueFastbootPacket: queueing packet failed (%d)",
    //              error);
  }
  return error;
}

/* WARNING: This is within interrupt context!
 * Queue the fastboot packets that were received
 */
extern "C" void OnFastbootPacketReceived(uint8_t const* buf, size_t len) {
  auto* packet = s_packet_allocator.MakePacketFromISR(buf, len);
  if (!packet) {
    // PW_LOG_ERROR(
    //     "OnFastbootPacketReceived: dropping packet of length=%d - allocation
    //     " "failed", len);
    return;
  }
  s_packets_in.QueueFromISR(packet);
}

/* WARNING : This is within interrupt context!
 * Frees packet buffers that have already been sent to the host.
 */
extern "C" void OnFastbootPacketSent(uint8_t const* buf, size_t len) {
  if (!buf || !len) {
    return;
  }

  // The pointer received from lower level code points to
  // the buffer, not the packet structure.
  auto* packet = Packet::from_data_pointer(const_cast<uint8_t*>(buf));
  s_packet_allocator.FreeFromISR(packet);

  // Check if there are any more packets queued and start sending
  // one immediately if available.
  if (auto* next_packet = s_packets_out.DequeueFromISR();
      next_packet != nullptr) {
    if (UsbEnqueueFastbootPacket(next_packet) != kStatus_USB_Success) {
      s_packet_allocator.FreeFromISR(next_packet);
    }
  }
}

void fastboot::mimxrt595evk::UsbTransportInit() { UsbInit(); }

ssize_t fastboot::mimxrt595evk::FastbootSendPacket(pw::ConstByteSpan to_send) {
  auto* packet = s_packet_allocator.MakePacket((uint8_t const*)to_send.data(),
                                               to_send.size());
  if (!packet) {
    PW_LOG_ERROR(
        "FastbootSendPacket failed: could not allocate buffer for in-flight "
        "packet");
    return -1;
  }

  PW_LOG_DEBUG("FastbootSendPacket: prepared packet=%p", packet);
  HexdumpWithAscii((uint8_t*)packet->data, packet->size);
  const volatile auto size = packet->size;

  // Critical section required to make UsbEnqueueFastbootPacket
  // and s_packets_out.Queue operations atomic.
  taskENTER_CRITICAL();
  const auto error = UsbEnqueueFastbootPacket(packet);
  // If there is a packet primed already, add the new one to the ISR queue
  // It will be picked up automatically after the existing one is already
  // transmitted to the host.
  if (error == kStatus_USB_Busy) {
    s_packets_out.Queue(packet);
    taskEXIT_CRITICAL();
    PW_LOG_DEBUG("FastbootSendPacket: queued packet=%p (USB busy)", packet);
    return size;
  } else if (error != kStatus_USB_Success) {
    s_packet_allocator.Free(packet);
    taskEXIT_CRITICAL();
    PW_LOG_ERROR("FastbootSendPacket failed: queueing packet failed (%d)",
                 error);
    return -2;
  }
  taskEXIT_CRITICAL();
  // Do not use `packet` from this point on! Use-after-free may occur if
  // an ISR happens immediately after queuing, and the ISR frees the
  // packet already.
  // This is fine, as we're only using the pointer and not dereferencing it.
  PW_LOG_DEBUG("FastbootSendPacket: packet %p sent", packet);
  return size;
}

ssize_t fastboot::mimxrt595evk::FastbootReceivePacket(pw::ByteSpan out) {
  while (true) {
    // Dequeue will block with portMAX_DELAY timeout, the
    // outer loop is only to handle the edge case if that
    // timeout were to actually expire.
    auto* packet = s_packets_in.Dequeue();
    if (!packet) {
      continue;
    }

    PW_LOG_DEBUG("FastbootReceivePacket: Received fastboot packet length=%d",
                 packet->size);
    HexdumpWithAscii(packet->data, packet->size);

    const auto copy_size = std::min(out.size(), packet->size);
    std::memcpy(out.data(), packet->data, copy_size);
    if (copy_size < packet->size) {
      PW_LOG_WARN(
          "FastbootReceivePacket: %d bytes were dropped due to buffer being "
          "too small!",
          packet->size - copy_size);
    }

    s_packet_allocator.Free(packet);
    return copy_size;
  }
}

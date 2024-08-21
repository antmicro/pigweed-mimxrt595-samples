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
#include <cstring>

#include "fastboot.h"
#include "pw_allocator/bucket_block_allocator.h"
#include "pw_allocator/buffer.h"
#include "pw_bytes/span.h"
#include "pw_hex_dump/hex_dump.h"
#include "pw_log/log.h"
#include "pw_status/status.h"

static const char FASTBOOT_RESPONSE_OKAY[] = "OKAY";

class PacketAllocator {
 public:
  PacketAllocator() { m_packet_allocator->Init(m_packet_allocator.as_bytes()); }

  void* Alloc(size_t len) {
    return m_packet_allocator->Allocate(pw::allocator::Layout(len));
  }

  void Free(void* ptr) { m_packet_allocator->Deallocate(ptr); }

 private:
  pw::allocator::WithBuffer<pw::allocator::BucketBlockAllocator<>, 0x1000>
      m_packet_allocator;
};

static PacketAllocator s_packet_allocator = {};

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

// WARNING: This is within interrupt context!
// The USB sample is built on callbacks, and this will be called directly
// from a USB ISR! This should optimally save the packet in a FreeRTOS queue
// instead and delegate responses to a separate task. Keep in mind that
// the lower-level USB code uses the same buffer for all reads, so the
// contents of `buf` would also have to be copied to an allocated temporary
// packet buffer.
extern "C" void OnFastbootPacketReceived(uint8_t const* buf, size_t len) {
  PW_LOG_DEBUG("OnFastbootPacketReceived: received fastboot packet length=%d",
               len);
  HexdumpWithAscii(buf, len);

  if (len > 0) {
    FastbootSendPacket((uint8_t const*)FASTBOOT_RESPONSE_OKAY,
                       sizeof(FASTBOOT_RESPONSE_OKAY));
  }
}

// WARNING: This is within interrupt context!
// Frees packet buffers that already have been sent to the host.
extern "C" void OnFastbootPacketSent(uint8_t const* buf, size_t len) {
  if (!buf || !len) {
    return;
  }
  s_packet_allocator.Free(const_cast<void*>((void const*)buf));
}

extern usb_cdc_vcom_struct_t s_cdcVcom;

extern "C" int FastbootSendPacket(uint8_t const* buf, size_t len) {
  auto* packet = s_packet_allocator.Alloc(len);
  if (!packet) {
    PW_LOG_ERROR(
        "FastbootSendPacket failed: could not allocate buffer for in-flight "
        "packet");
    return -1;
  }
  std::memcpy(packet, buf, len);

  const auto error =
      USB_DeviceCdcAcmSend(s_cdcVcom.cdcAcmHandle,
                           FASTBOOT_USB_BULK_IN_ENDPOINT,
                           const_cast<uint8_t*>((uint8_t*)packet),
                           len);
  if (error != kStatus_USB_Success) {
    s_packet_allocator.Free(packet);
    PW_LOG_ERROR("FastbootSendPacket failed: queueing packet failed (%d)",
                 error);
    return -2;
  }

  PW_LOG_DEBUG("FastbootSendPacket: queued packet=%p", packet);
  HexdumpWithAscii((uint8_t*)packet, len);

  return 0;
}

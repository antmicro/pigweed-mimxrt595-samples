#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <algorithm>

#include "pw_fastboot/transport.h"
#include "pw_stream/memory_stream.h"

extern "C" {

/* Inform that a fastboot packet was received from the host.
 *
 * This must be called from the USB sample.
 */
void OnFastbootPacketReceived(uint8_t const* buf, size_t len);

/* Inform that a previously queued packet was sent to the host.
 *
 * This must be called from the USB sample.
 */
void OnFastbootPacketSent(uint8_t const* buf, size_t len);
}

#include "pw_bytes/span.h"

namespace fastboot::mimxrt595evk {

/* Initialize the USB fastboot transport
 *
 * This must be called before sending any packets using
 * FastbootSendPacket and FastbootReceivePacket.
 */
void UsbTransportInit();

/* Enqueue a fastboot packet to send back to the host.
 *
 * The caller must be within a FreeRTOS task!
 */
ssize_t FastbootSendPacket(pw::ConstByteSpan);

/* Read a fastboot packet from the host into the specified ByteSpan.
 *
 * The caller must be within a FreeRTOS task!
 */
ssize_t FastbootReceivePacket(pw::ByteSpan);

/* Implementation of the Transport interface for using fastboot
 * with USB on MIMXRT595-EVK devboard.
 */
class UsbTransport : public pw::fastboot::Transport {
 public:
  UsbTransport() { fastboot::mimxrt595evk::UsbTransportInit(); }

  ssize_t Read(void* data, size_t len) override {
    auto* byte_data = (std::byte*)data;
    size_t bytes_read_total = 0;
    while (bytes_read_total < len) {
      // Limit read to maximum we can receive from a HS USB packet (512 bytes)
      const auto bytes_to_read = std::min(len - bytes_read_total, 512U);
      const auto bytes_read_now = fastboot::mimxrt595evk::FastbootReceivePacket(
          pw::ByteSpan{byte_data, bytes_to_read});
      if (bytes_read_now < 0) {
        return bytes_read_total == 0 ? -1 : bytes_read_total;
      }
      bytes_read_total += bytes_read_now;
      byte_data += bytes_read_now;
      if (static_cast<size_t>(bytes_read_now) < bytes_to_read) {
        break;
      }
    }
    return bytes_read_total;
  }

  // Writes |len| bytes from |data|. Returns the number of bytes actually
  // written or -1 on error.
  ssize_t Write(const void* data, size_t len) override {
    auto const* byte_data = (std::byte const*)data;
    size_t bytes_written_total = 0;
    while (bytes_written_total < len) {
      // Limit write to maximum we can write in a HS USB packet (512 bytes)
      const auto bytes_to_write = std::min(len - bytes_written_total, 512U);
      const auto bytes_written_now = fastboot::mimxrt595evk::FastbootSendPacket(
          pw::ConstByteSpan{byte_data, bytes_to_write});
      if (bytes_written_now < 0) {
        return bytes_written_total == 0 ? -1 : bytes_written_total;
      }
      bytes_written_total += bytes_written_now;
      byte_data += bytes_written_now;
      if (static_cast<size_t>(bytes_written_now) < bytes_to_write) {
        break;
      }
    }
    return bytes_written_total;
  }

  // Closes the underlying transport. Returns 0 on success.
  int Close() override { return 0; }

  int Reset() override { return 0; }
};

}  // namespace fastboot::mimxrt595evk

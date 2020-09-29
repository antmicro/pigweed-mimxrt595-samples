
#include <span>

#include "pw_hdlc_lite/encoder.h"
#include "pw_hdlc_lite/sys_io_stream.h"
#include "pw_log/levels.h"
#include "pw_log_tokenized/log_tokenized.h"
#include "pw_tokenizer/base64.h"
#include "pw_tokenizer/config.h"

namespace {
pw::stream::SysIoWriter writer;
}  // namespace

// The tokenizer handler implementation below exemplifies how to build a
// tokenized logging backend that encodes log entries in HDLC frames sent to an
// arbitrary RPC channel.
// NOTE: this is not the recommended way to customize the backend.
extern "C" void pw_tokenizer_HandleEncodedMessageWithPayload(
    pw_tokenizer_Payload payload,
    const uint8_t log_buffer[],
    size_t size_bytes) {

  pw::log_tokenized::Metadata metadata = payload;

  // Encode the tokenized message as Base64.
  char base64_buffer[PW_TOKENIZER_CFG_ENCODING_BUFFER_SIZE_BYTES * 4 / 3 + 1];
  const size_t base64_bytes = pw::tokenizer::PrefixedBase64Encode(
      std::span(log_buffer, size_bytes), base64_buffer);
  base64_buffer[base64_bytes] = '\0';

  // HDLC encode the Base64 string and send to RPC channel 1 via a SysIoWriter.
  if (metadata.level() >= PW_LOG_LEVEL_DEBUG) {
    pw::hdlc_lite::WriteInformationFrame(
        /*address=*/1,
        std::as_bytes(std::span(base64_buffer, base64_bytes)),
        writer);
  }
}

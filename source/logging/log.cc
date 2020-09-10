
#include <span>
#include <string_view>

#include "pw_log/levels.h"
#include "pw_log_tokenized/log_tokenized.h"
#include "pw_tokenizer/base64.h"
#include "pw_tokenizer/config.h"
#include "pw_sys_io/sys_io.h"

// The tokenizer handler implementation below exemplifies how to build a
// tokenizer backend. However, this is not the recommended way to customize the
// backend.
// TODO(pwbug/264): move this implementation to upstream Pigweed as an optional
// pw_log_basic backend.
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

  // Print the Base64 string.
  if (metadata.level() >= PW_LOG_LEVEL_DEBUG) {
    pw::sys_io::WriteLine(std::string_view(base64_buffer, base64_bytes));
  }
}


#include <span>

#include "pw_log/levels.h"
#include "pw_log_tokenized/log_tokenized.h"
#include "pw_sys_io/sys_io.h"
#include "pw_tokenizer/base64.h"

extern "C" void pw_tokenizer_HandleEncodedMessageWithPayload(
    pw_tokenizer_Payload payload, const uint8_t message[], size_t size) {
  // The metadata object provides the log level, module token, and flags.
  // These values can be recorded and used for runtime filtering.
  pw::log_tokenized::Metadata metadata(payload);

  // if (metadata.level() < current_log_level) {
  //   return;
  // }

  // if (metadata.flags() & HIGH_PRIORITY_LOG != 0) {
  //   EmitHighPriorityLog(metadata.module(), message, size);
  // } else {
  //   EmitLowPriorityLog(metadata.module(), message, size);
  // }

  // TODO(tonymd): Find out correct way to output message. So that running this
  // command works: python -m pw_tokenizer.serial_detokenizer
  //   --device /dev/ttyACM0 --baudrate 115200
  //   workshop/01-blinky/tokenizer_database.csv

  pw::sys_io::WriteBytes(std::as_bytes(std::span(message, size)));
}

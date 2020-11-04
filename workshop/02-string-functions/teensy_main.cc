#include <Arduino.h>
#include <TimeLib.h>

#include <array>
#include <span>

#include "pw_hex_dump/hex_dump.h"
#include "pw_log/log.h"
#include "pw_span/internal/span.h"
#include "pw_string/format.h"
#include "pw_string/string_builder.h"

time_t getTeensy3Time() { return Teensy3Clock.get(); }

int main() {
  setSyncProvider(getTeensy3Time);
  if (timeStatus() != timeSet) {
    PW_LOG_INFO("Teensy Time: Unable to sync with the RTC.");
  } else {
    PW_LOG_INFO("Teensy Time: RTC has set the system time.");
  }

  constexpr int kLedPin = 13;
  pinMode(kLedPin, OUTPUT);

  const char my_data[] = "Super Simple Time Logging!";
  std::array<char, 80> hex_dump_buffer;
  pw::dump::FormattedHexDumper hex_dumper(hex_dump_buffer);
  hex_dumper.BeginDump(std::as_bytes(std::span(my_data)));
  while (hex_dumper.DumpLine().ok()) {
    PW_LOG_INFO("%s", hex_dump_buffer.data());
  }

  // std::byte buffer[64];
  // pw::StringBuilder sb(buffer);

  char buffer[64];

  while (true) {
    pw::string::Format(buffer,
                       "%d-%02d-%02d %02d:%02d:%02d ",
                       year(),
                       month(),
                       day(),
                       hour(),
                       minute(),
                       second());
    PW_LOG_INFO("%s", buffer);
    delay(1000);
    // sb.Format("0x%x", 0xabc).ok();
    // PW_LOG_INFO(sb.data());
    // delay(1000);
    // sb.Format("GHI").ok();
    // PW_LOG_INFO(sb.data());
    // delay(1000);
  }

  return 0;
}

// Copyright 2020 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include <Arduino.h>
#include <TimeLib.h>

#include <array>

#include "pw_hex_dump/hex_dump.h"
#include "pw_log/log.h"
#include "pw_span/internal/span.h"
#include "pw_span/span.h"
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
  hex_dumper.BeginDump(pw::as_bytes(pw::span(my_data)));
  while (hex_dumper.DumpLine().ok()) {
    PW_LOG_INFO("%s", hex_dump_buffer.data());
  }

  char buffer[64];

  unsigned update_count = 0;

  while (true) {
    pw::string::Format(buffer,
                       "[%d-%02d-%02d %02d:%02d:%02d] Message number: %u",
                       year(),
                       month(),
                       day(),
                       hour(),
                       minute(),
                       second(),
                       update_count);
    PW_LOG_INFO("%s", buffer);
    delay(1000);

    update_count = (update_count + 1) % 65535;
  }

  return 0;
}

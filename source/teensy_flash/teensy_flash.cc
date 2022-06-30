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

#include "teensy_flash/teensy_flash.h"

#include <algorithm>

#include "pw_kvs/alignment.h"
#include "pw_log/log.h"

#ifdef __x86_64__
#error x86 is not supported!
#endif  // __x86_64__

extern "C" {
// Requires running the following command after installing the arduino core:
//
// sed -i -e 's/^static void flash_/void flash_/'
// third_party/pigweed/third_party/arduino/cores/teensy/hardware/teensy/avr/cores/teensy4/eeprom.c
extern void flash_write(void* addr, const void* data, uint32_t len);
extern void flash_erase_sector(void* addr);
}

namespace sample {

// Erase num_sectors starting at a given address.
pw::Status TeensyFlashMemory::Erase(Address address, size_t num_sectors) {
  // Erase each sector, one at a time.
  for (size_t i = 0; i < num_sectors; i++) {
    void* sector_address = (void*)(address + (i * sector_size_bytes()));

    // Requires running the following command after installing the arduino core:
    //
    // sed -i -e 's/^static void flash_/void flash_/'
    // third_party/pigweed/third_party/arduino/cores/teensy/hardware/teensy/avr/cores/teensy4/eeprom.c
    flash_erase_sector(sector_address);  // See sed comment above
  }

  return pw::OkStatus();
}

// Reads bytes from flash into buffer.
pw::StatusWithSize TeensyFlashMemory::Read(Address address,
                                           pw::span<std::byte> output) {
  memcpy(output.data(), (void*)((size_t)address), output.size_bytes());
  return pw::StatusWithSize(output.size_bytes());
}

// Writes bytes to flash.
pw::StatusWithSize TeensyFlashMemory::Write(Address address,
                                            pw::span<const std::byte> data) {
  pw::span<const std::byte> write_data = data;

  // Writing using flash_write is done by upto page size chunks. Each call to
  // flash_write needs to fit in a page.
  while (write_data.size_bytes() > 0) {
    size_t bytes_remaining_in_page =
        (size_t)(pw::AlignUp(address, kWritePageSize) - address);
    if (bytes_remaining_in_page == 0) {
      bytes_remaining_in_page = kWritePageSize;
    }

    size_t chunk_size =
        std::min(bytes_remaining_in_page, write_data.size_bytes());

    // Requires running the following command after installing the arduino core:
    //
    // sed -i -e 's/^static void flash_/void flash_/'
    // third_party/pigweed/third_party/arduino/cores/teensy/hardware/teensy/avr/cores/teensy4/eeprom.c
    flash_write((void*)(address),
                write_data.data(),
                chunk_size);  // See sed comment above

    write_data = write_data.subspan(chunk_size);
    address += chunk_size;
  }

  return pw::StatusWithSize(data.size_bytes());
}

}  // namespace sample

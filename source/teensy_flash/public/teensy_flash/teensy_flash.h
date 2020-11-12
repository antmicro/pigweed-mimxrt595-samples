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

#pragma once

#include "pw_kvs/flash_memory.h"

namespace sample {

class TeensyFlashMemory : public pw::kvs::FlashMemory {
 public:
  TeensyFlashMemory()
      : FlashMemory(kSectorSize, kSectorCount, kAlignment, kStartAddress) {}

  pw::Status Enable() override { return pw::Status::Ok(); }

  pw::Status Disable() override { return pw::Status::Ok(); }

  bool IsEnabled() const override { return true; }

  // Erase num_sectors starting at a given address.
  pw::Status Erase(Address address, size_t num_sectors) override;

  // Reads bytes from flash into buffer.
  pw::StatusWithSize Read(Address address,
                          std::span<std::byte> output) override;

  // Writes bytes to flash.
  pw::StatusWithSize Write(Address address,
                           std::span<const std::byte> data) override;

  std::byte* FlashAddressToMcuAddress(Address address) const override {
    return (std::byte*)address;
  };

 private:
#if defined(ARDUINO_TEENSY40)
#define FLASH_BASEADDR 0x601F0000
#define FLASH_SECTORS 15
#elif defined(ARDUINO_TEENSY41)
#define FLASH_BASEADDR 0x607C0000
#define FLASH_SECTORS 63
#else
#error Core not supported...
#endif

  static constexpr size_t kWritePageSize = 256;
  static constexpr size_t kSectorSize = (4 * 1024);
  static constexpr size_t kSectorCount = FLASH_SECTORS;
  static constexpr size_t kAlignment = 1;
  static constexpr size_t kStartAddress = FLASH_BASEADDR;
};

}  // namespace sample

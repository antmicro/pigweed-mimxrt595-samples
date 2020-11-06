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

#include "pw_board_led/led.h"
#include "pw_kvs/fake_flash_memory.h"
#include "pw_kvs/key_value_store.h"
#include "pw_log/log.h"
#include "pw_spin_delay/delay.h"

constexpr size_t kSectorSizeBytes = 512;
constexpr size_t kSectorCount = 4;
constexpr size_t kKvsMaxEntries = 32;

// Use ram-backed (simulated) FlashMemory for storage.
pw::kvs::FakeFlashMemoryBuffer<kSectorSizeBytes, kSectorCount> example_memory;

// Use a single flash partition that covers the entire flash memory.
pw::kvs::FlashPartition example_partition(&example_memory);

// For KVS magic value always use a random 32 bit integer rather than a human
// readable 4 bytes. See pw_kvs/format.h for more information.
static constexpr pw::kvs::EntryFormat kvs_format = {.magic = 0xd253a8a9,
                                                    .checksum = nullptr};

pw::kvs::KeyValueStoreBuffer<kKvsMaxEntries, kSectorCount> kvs(
    &example_partition, kvs_format);

int main() {
  pw::board_led::Init();

  unsigned kvs_value = 42;

  kvs.Init();

  while (true) {
    pw::board_led::TurnOn();
    pw::spin_delay::WaitMillis(2000);

    PW_LOG_INFO("Write KVS value of %u", kvs_value);
    kvs.Put("example_key", kvs_value);

    PW_LOG_INFO("KVS has %u keys", static_cast<unsigned>(kvs.size()));

    pw::board_led::TurnOff();
    pw::spin_delay::WaitMillis(2000);

    unsigned read_value = 0;
    kvs.Get("example_key", &read_value);
    PW_LOG_INFO("Read KVS value of %u", read_value);

    kvs_value++;
    if (kvs_value > 100) {
      kvs_value = 0;
    }
  }

  return 0;
}

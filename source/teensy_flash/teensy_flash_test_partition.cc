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

#include "pw_kvs/flash_test_partition.h"
#include "teensy_flash/teensy_flash.h"

namespace pw::kvs {
using sample::TeensyFlashMemory;

namespace {

TeensyFlashMemory test_flash;

// Partition using the teensy flash. Use the whole flash with an alignment
// of 16.
FlashPartition test_partition(&test_flash, 0, test_flash.sector_count(), 1);

}  // namespace

FlashPartition& FlashTestPartition() { return test_partition; }

}  // namespace pw::kvs

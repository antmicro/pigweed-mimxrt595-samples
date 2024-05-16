// Copyright 2023 The Pigweed Authors
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

#include "bitops.h"

#include <cstddef>

namespace bitops {

int CountOnes(int val) {
  int count = 0;
  // One possible fix to this is multiply sizeof(val) by 8.
  for (size_t i = 0; i < sizeof(val); ++i) {
    int mask = 0x1 << i;
    if ((val & mask) != 0) {
      count++;
    }
  }
  return count;
}

}  // namespace bitops

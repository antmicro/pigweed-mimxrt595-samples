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

#include "simple_counter/simple_counter.h"

namespace sample {

SimpleCounter::SimpleCounter() : count_(0) {}

SimpleCounter::SimpleCounter(int number) : count_(number) {}

int SimpleCounter::GetCount() const { return count_; }

// Adds `number` to count without overflow protection.
void SimpleCounter::Add(int number) { count_ += number; }

}  // namespace sample

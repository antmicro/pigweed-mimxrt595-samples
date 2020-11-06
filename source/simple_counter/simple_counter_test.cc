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

#include "gtest/gtest.h"

namespace sample {

TEST(SimpleCounterTests, Increment) {
  SimpleCounter counter;
  EXPECT_EQ(counter.GetCount(), 0);

  // Add an arbitrary number.
  counter.Add(33);
  EXPECT_EQ(counter.GetCount(), 33);

  // Subtract an arbitrary number.
  counter.Add(-4);
  EXPECT_EQ(counter.GetCount(), 29);
}

}  // namespace sample

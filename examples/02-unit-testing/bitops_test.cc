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

#include "gtest/gtest.h"

namespace bitops {
namespace {

TEST(Bitops, SomeOnes) { ASSERT_EQ(CountOnes(1), 1); }

TEST(Bitops, MoreOnes) { ASSERT_EQ(CountOnes(0x0f), 4); }

TEST(Bitops, EvenMoreOnes) { ASSERT_EQ(CountOnes(0b11100101), 5); }

TEST(Bitops, NoOnes) { ASSERT_EQ(CountOnes(0), 0); }

}  // namespace
}  // namespace bitops

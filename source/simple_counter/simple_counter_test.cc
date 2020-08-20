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

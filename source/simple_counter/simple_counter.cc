#include "simple_counter/simple_counter.h"

namespace sample {

SimpleCounter::SimpleCounter() : count_(0) {}

SimpleCounter::SimpleCounter(int number) : count_(number) {}

int SimpleCounter::GetCount() const { return count_; }

// Adds `number` to count without overflow protection.
void SimpleCounter::Add(int number) { count_ += number; }

}  // namespace sample

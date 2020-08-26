#include "simple_counter/simple_counter.h"

#include "pw_assert/assert.h"
#include "pw_log/log.h"

int main() {
  // Use sample Module
  sample::SimpleCounter counter(3);
  counter.Add(-1);
  // Sample assert. Adding a message is optional.
  PW_CHECK_INT_EQ(counter.GetCount(), 2);
  counter.Add(10);

  // Chance this value to see the assert below crash the app.
  const int expected_count = 12;

  // Use sample assert with a message.
  PW_CHECK_INT_EQ(counter.GetCount(), expected_count,
                  "Counter value %d != %d as expected",
                  counter.GetCount(), expected_count);

  PW_LOG_INFO("Hello World!");

  return 0;
}

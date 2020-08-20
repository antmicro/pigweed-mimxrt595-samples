#include "simple_counter/simple_counter.h"

#include "pw_log/log.h"

int main() {
  // Use sample Module
  sample::SimpleCounter counter(3);
  counter.Add(-1);
  counter.Add(10);

  if (counter.GetCount() != 12) {
    return 1;
  }

  PW_LOG_INFO("Hello World!");

  return 0;
}

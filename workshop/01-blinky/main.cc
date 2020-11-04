#include "pw_board_led/led.h"
#include "pw_log/log.h"

namespace {

// This is a bad spin delay. It's not accurate, and doesn't correspond with any
// real time unit. pw_chrono is coming soon, and will make it easier to do this
// correctly!
void Delay(size_t ticks) {
  for (volatile size_t i = 0; i < ticks; i++) {
    // Do nothing.
  }
}

}  // namespace

int main() {
  pw::board_led::Init();
  while (true) {
    PW_LOG_INFO("Blink High!");
    pw::board_led::TurnOn();
    Delay(1000000);
    PW_LOG_INFO("Blink Low!");
    pw::board_led::TurnOff();
    Delay(1000000);
  }

  return 0;
}

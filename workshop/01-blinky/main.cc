#include <Arduino.h>

#include "pw_log/log.h"

int main() {
  constexpr int kLedPin = 13;
  pinMode(kLedPin, OUTPUT);

  while (true) {
    PW_LOG_INFO("Blink High!");
    digitalWrite(kLedPin, HIGH);
    delay(1000);
    PW_LOG_INFO("Blink Low!");
    digitalWrite(kLedPin, LOW);
    delay(1000);
  }

  return 0;
}

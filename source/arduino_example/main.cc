#include <Arduino.h>

#include "pw_log/log.h"

int main() {
  PW_LOG_INFO("Blink!");

  constexpr int kLedPin = 13;
  pinMode(kLedPin, OUTPUT);

  while (true) {
    digitalWrite(kLedPin, HIGH);
    delay(1000);
    digitalWrite(kLedPin, LOW);
    delay(1000);
  }

  return 0;
}

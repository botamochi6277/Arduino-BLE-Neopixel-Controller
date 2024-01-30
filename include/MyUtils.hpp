#ifndef BOTALAB_UTILS_HPP
#define BOTALAB_UTILS_HPP

#include <Arduino.h>

void init_leds(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b) {
  pinMode(pin_r, OUTPUT);
  pinMode(pin_g, OUTPUT);
  pinMode(pin_b, OUTPUT);
  // turn off the all build-in leds
  digitalWrite(pin_r, HIGH);
  digitalWrite(pin_g, HIGH);
  digitalWrite(pin_b, HIGH);
}

#endif
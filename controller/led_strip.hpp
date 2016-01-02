//
// Created by Aaron on 12/31/2015.
//

#ifndef CONTROLLER_LED_STRIP_H
#define CONTROLLER_LED_STRIP_H

#define FASTLED_INTERNAL //Silences dumb "warnings" for version number

#include <FastLED.h>

#include "pin_config.hpp"

#define MY_STRIP_COLOR_CORRECTION 0xFFB0A0

extern CRGB LED_STRIP[NUM_LEDS];

void initialize_led_strip();

#endif //CONTROLLER_LED_STRIP_H

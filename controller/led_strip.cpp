//
// Created by Aaron on 12/31/2015.
//

#include "led_strip.h"

CRGB LED_STRIP[NUM_LEDS];

void initialize_led_strip() {
    FastLED.addLeds<WS2812, LED_DATA_PIN, GRB>(LED_STRIP, NUM_LEDS);

    FastLED.setCorrection(MY_STRIP_COLOR_CORRECTION);
}
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <ArduinoThread/Thread.h>
#include <ArduinoThread/ThreadController.h>

#include <FastLED/FastLED.h>

#define NUM_LEDS 144
#define LED_STRIP_PIN  16

int LCD_RS = 52,
    LCD_ENABLE = 53;

int LCD_D[4] = {48, 49, 50, 51};

LiquidCrystal* lcd;


void setup() {
    pinMode(13, OUTPUT);

    lcd = new LiquidCrystal(LCD_RS, LCD_ENABLE, LCD_D[0], LCD_D[1], LCD_D[2], LCD_D[3]);

    lcd->begin(20, 4);
    lcd->clear();
    lcd->print("Ready for input");
}

void loop() {
    digitalWrite(13, 1);
    delay(500);
    digitalWrite(13, 0);
    delay(500);
}
#include <Arduino.h>

#include <ArduinoThread/Thread.h>

void setup() {
    pinMode(13, OUTPUT);
}

void loop() {
    digitalWrite(13, 1);
    delay(500);
    digitalWrite(13, 0);
    delay(500);
}
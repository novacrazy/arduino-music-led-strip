#include <Arduino.h>
#include <LiquidCrystal.h>

#include "pin_config.h"

#include "command_protocol.h"

#include "commands.h"

#include "led_strip.h"

LiquidCrystal *lcd;
CommandProtocol *protocol;
float channels[3];

static Command request_command(COMMAND_REQUEST_ACTION);

float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void commandCallback(Command *cmd) {
    switch (cmd->getCommand()) {
        case COMMAND_ECHO: {
            cmd->setCommand(COMMAND_INFO);

            protocol->sendCommand(*cmd, commandCallback);

            break;
        }
        case COMMAND_NO_OP:
        case COMMAND_INFO: {
            //Discard
            break;
        }
        case COMMAND_WRITE_STEREO: {
            memcpy(channels, cmd->getData(), sizeof(float) * 3);

            float max_recorded = channels[2];

            analogWrite(LEFT_LED_PIN, mapf(channels[0], 0, max_recorded, 0, 255));
            analogWrite(RIGHT_LED_PIN, mapf(channels[1], 0, max_recorded, 0, 255));

            break;
        }
        default: {
            digitalWrite(13, HIGH);

            Serial.print(F("Unknown Command"));
        }
    }
}

void setup() {
    delay(500); //for power overload protection

    Serial.begin(256000, SERIAL_8E2);

    while (!Serial) {
        delay(100);
    }

    protocol = new CommandProtocol();

    protocol->start(commandCallback);

    initialize_led_strip();

    FastLED.setDither(BINARY_DITHER);
    //FastLED.setDither(DISABLE_DITHER);
    FastLED.setBrightness(30);

    lcd = new LiquidCrystal(LCD_PINS_ALL);

    lcd->begin(20, 4);
    lcd->clear();

    Serial.print(F("Initialized"));
}

void showLEDs() {
    int channel_count = NUM_LEDS / 2;

    float whole, max_recorded = channels[2];

    float left_max = mapf(channels[0], 0, max_recorded, 0, channel_count);
    float right_max = NUM_LEDS - mapf(channels[1], 0, max_recorded, 0, channel_count);

    CRGB color = CRGB::Blue;
    CRGB black = CRGB::Black;

    for (int i = 0; i < channel_count; i++) {
        if (left_max > i) {
            LED_STRIP[i] = color;

        } else {
            LED_STRIP[i] = black;
        }
    }

    for (int i = 0; i < NUM_LEDS - 2; i++) {
        LED_STRIP[i + 1] = LED_STRIP[i].lerp8(LED_STRIP[i + 1], 127);
    }

    for (int i = NUM_LEDS - 1; i >= channel_count; i--) {
        if (right_max < i) {
            LED_STRIP[i] = color;

        } else {
            LED_STRIP[i] = black;
        }
    }

    for (int i = NUM_LEDS - 1; i > channel_count; i--) {
        LED_STRIP[i - 1] = LED_STRIP[i].lerp8(LED_STRIP[i - 1], 127);
    }


    FastLED.delay(10);
}

void loop() {
    protocol->run();

    if (!protocol->isWaiting()) {
        showLEDs();

        bool res = protocol->sendCommand(request_command);

        if (!res) {
            Serial.write("Error");
        }
    }
}

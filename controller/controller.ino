#include <Arduino.h>
#include <LiquidCrystal.h>

#include "pin_config.hpp"

#include "command_protocol.hpp"

#include "commands.hpp"

#include "led_strip.hpp"

#include "math.hpp"

LiquidCrystal *lcd;
CommandProtocol *protocol;
float channels[3];

static Command request_command(COMMAND_REQUEST_ACTION);

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

            break;
        }
        default: {
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

    float left_max = fscale(channels[0], 0, max_recorded, 0, channel_count, 2);
    float right_max = NUM_LEDS - fscale(channels[1], 0, max_recorded, 0, channel_count, 2) - 1;

    fill_rainbow(LED_STRIP, NUM_LEDS, 0, 255 / NUM_LEDS);

    for (int i = 0; i < channel_count; i++) {
        if (left_max <= i) {
            LED_STRIP[i] = CRGB::Black;
        }
    }

    for (int i = NUM_LEDS - 1; i >= channel_count; i--) {
        if (right_max >= i) {
            LED_STRIP[i] = CRGB::Black;
        }
    }

    blur1d(LED_STRIP, NUM_LEDS, 160);

    analogWrite(LEFT_LED_PIN, fmap(channels[0], 0, max_recorded, 0, 255));
    analogWrite(RIGHT_LED_PIN, fmap(channels[1], 0, max_recorded, 0, 255));

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

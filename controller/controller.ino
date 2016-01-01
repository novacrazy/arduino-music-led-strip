#include <Arduino.h>
#include <LiquidCrystal.h>

#include "pin_config.h"

#include "led_strip.h"

#include "command_protocol.h"

#include "commands.h"

LiquidCrystal *lcd;
CommandProtocol *protocol;

void commandCallback(Command *cmd) {
    switch (cmd->getCommand()) {
        case COMMAND_ECHO: {
            cmd->setCommand(COMMAND_INFO);

            protocol->sendRequest(*cmd, commandCallback);

            break;
        }
        case COMMAND_INFO: {
            //Discard info
            break;
        }
        default: {
            Serial.print(F("Unknown Command"));
        }
    }
}

void setup() {
    Serial.begin(9600);

    while (!Serial) {
        delay(100);
    }

    //pinMode(13, OUTPUT);

    lcd = new LiquidCrystal(LCD_PINS_ALL);

    lcd->begin(20, 4);
    lcd->clear();
    lcd->print(F("Ready for input"));

    //initialize_led_strip();

    //FastLED.setDither(BINARY_DITHER);
    //FastLED.setDither(DISABLE_DITHER);
    //FastLED.setBrightness(25);

    protocol = new CommandProtocol();

    protocol->start();

    Serial.print(F("Initialized"));
}

static Command request_command(COMMAND_REQUEST_ACTION);

void loop() {
    protocol->yield();

    if (!protocol->isWaiting()) {
        bool res = protocol->sendRequest(request_command, commandCallback);

        if (!res) {
            Serial.print("Error");
        }
    }

    delay(10);
}

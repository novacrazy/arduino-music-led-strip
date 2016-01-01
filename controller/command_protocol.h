//
// Created by Aaron on 12/31/2015.
//

#ifndef CONTROLLER_COMMAND_PROTOCOL_H
#define CONTROLLER_COMMAND_PROTOCOL_H

#include <Arduino.h>

#ifndef COMMAND_PROTOCOL_DEFAULT_BUFFER_SIZE
#define COMMAND_PROTOCOL_DEFAULT_BUFFER_SIZE 256
#endif

#ifndef COMMAND_PROTOCOL_DEFAULT_WAIT_TIME
#define COMMAND_PROTOCOL_DEFAULT_WAIT_TIME 100
#endif

class CommandProtocol;

class Command {
protected:
    uint8_t cmd, size, *data;

public:
    Command(uint8_t, uint8_t = 0);

    uint8_t &operator[](uint8_t);

    void setData(uint8_t *);

    uint8_t *getData();

    uint8_t getCommand() const;

    uint8_t getSize() const;

    friend class CommandProtocol;
};

/*
 * Basic usage:
 *
 * void myCallback(Command* cmd) {
 *      //only re-enable LED strip when no commands are being sent or received
 *      enableLEDStrip();
 *
 *      //do stuff
 * }
 *
 * while(true) {
 *      if(protocol.isWaiting()) {
 *          protocol.yield();
 *
 *      } else {
 *          disableLEDStrip();
 *
 *          //Usually a request for data is sent
 *          protocol.sendRequest(&request_command, &myCallback);
 *      }
 * }
 *
 * */

struct CommandProtocolMagic {
    CommandProtocolMagic(const char *const _s, uint8_t _m)
        : _start_message(_s), _magic_header(_m) { }

    const char *const _start_message;
    uint8_t _magic_header;
};

extern CommandProtocolMagic default_command_protocol_magic;

class CommandProtocol {
protected:
    static uint8_t default_buffer[COMMAND_PROTOCOL_DEFAULT_BUFFER_SIZE];

private:
    Stream *const stream;

    const CommandProtocolMagic *magic;

    struct _Next {
        uint8_t stage;
        Command command;
        bool waiting;

        const void (*callback)(Command *);
    } next{0, {0, 0}, false, NULL};

    bool initialized;

public:
    CommandProtocol(Stream *const = &Serial,
                    const CommandProtocolMagic * = &default_command_protocol_magic);

    void start();

    bool sendRequest(const Command *, const void (*)(Command *));

    bool isWaiting() const;

    //This should be called as often as possible
    void yield();
};

#endif //CONTROLLER_COMMAND_PROTOCOL_H

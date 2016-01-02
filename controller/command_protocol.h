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

    void setCommand(uint8_t);

    uint8_t getSize() const;

    void setSize(uint8_t);

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
 *          protocol.sendCommand(request_command, myCallback);
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
public:
    typedef void (*callback_t)(Command *);

protected:
    static uint8_t default_buffer[COMMAND_PROTOCOL_DEFAULT_BUFFER_SIZE];

private:
    Stream *const stream;

    const CommandProtocolMagic *magic;

    struct _Next {
        uint8_t stage, count;
        Command command;
        bool waiting;
    } next{0, 0, {0, 0}, false};

    callback_t callback;

    bool initialized;

public:
    CommandProtocol(Stream *const = &Serial,
                    const CommandProtocolMagic * = &default_command_protocol_magic);

    void start(callback_t = NULL);

    bool sendCommand(const Command &);

    bool sendCommand(const Command &, callback_t);

    void awaitCommand(callback_t = NULL);

    void setCallback(callback_t);

    callback_t getCallback() const;

    bool isWaiting() const;

    //This should be called as often as possible
    void run();
};

#endif //CONTROLLER_COMMAND_PROTOCOL_H

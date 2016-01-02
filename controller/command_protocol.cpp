//
// Created by Aaron on 1/1/2016.
//

#include "command_protocol.h"

CommandProtocolMagic default_command_protocol_magic("Hello, World!", 0xFF);

Command::Command(uint8_t cmd, uint8_t size)
    : cmd(cmd), size(size) {
}

uint8_t &Command::operator[](uint8_t index) {
    return this->data[index];
}

void Command::setData(uint8_t *data) {
    this->data = data;
}

uint8_t *Command::getData() {
    return this->data;
}

uint8_t Command::getCommand() const {
    return this->cmd;
}

void Command::setCommand(uint8_t cmd) {
    this->cmd = cmd;
}

uint8_t Command::getSize() const {
    return this->size;
}

void Command::setSize(uint8_t size) {
    this->size = size;
}

uint8_t CommandProtocol::default_buffer[COMMAND_PROTOCOL_DEFAULT_BUFFER_SIZE] = {0x0};

CommandProtocol::CommandProtocol(Stream *const stream, const CommandProtocolMagic *magic)
    : stream(stream), initialized(false), magic(magic) {
}

void CommandProtocol::start(callback_t callback) {
    if (callback != NULL) {
        this->setCallback(callback);
    }

    if (!this->initialized) {
        uint8_t it = 0, stage = 0, len = strlen(magic->_start_message);

        bool do_loop = true;

        while (do_loop) {
            switch (stage) {
                case 0: {
                    this->stream->write(this->magic->_start_message);
                    stage++;
                    break;
                }
                case 1: {
                    while (this->stream->available()) {
                        uint8_t next = this->stream->read();

                        if (this->magic->_start_message[it] == next) {
                            it++;

                            if (it == len) {
                                stage++;
                                break;
                            }

                        } else {
                            it = 0;
                        }
                    }
                    break;
                }
                case 2: {
                    this->initialized = true;
                    do_loop = false;
                    break;
                }
            }

            delay(COMMAND_PROTOCOL_DEFAULT_WAIT_TIME);
        }
    }
}

bool CommandProtocol::sendCommand(const Command &request) {
    if (this->isWaiting()) {
        return false;

    } else {
        size_t sent = 0;

        if (request.size > 0 && request.data != NULL) {
            return false;
        }

        sent += this->stream->write(this->magic->_magic_header);

        sent += this->stream->write((uint8_t *) (&request), 2);

        if (request.size > 0) {
            sent += this->stream->write(request.data, request.size);
        }

        if (sent == request.size + 3) {
            this->next.waiting = true;
            return true;

        } else {
            return false;
        }
    }
}

void CommandProtocol::awaitCommand(CommandProtocol::callback_t callback) {
    if (callback != NULL) {
        this->setCallback(callback);
    }

    this->next.waiting = true;
}

bool CommandProtocol::isWaiting() const {
    return this->next.waiting;
}

void CommandProtocol::run() {
    if (next.waiting && this->stream->available()) {
        switch (next.stage) {
            case 0: {
                auto c = this->stream->read();

                if (c == magic->_magic_header) {
                    next.stage++;
                }

                next.count = 0;

                break;
            }
            case 1: {
                //This seems a bit contrived, but it's just so reads can be bundled together as one operation

                if (this->stream->available() >= 2 && next.count == 0) {
                    this->stream->readBytes((uint8_t *) (&next.command), 2);

                } else if (next.count < 2) {
                    if (next.count == 0) {
                        next.command.cmd = (uint8_t) (this->stream->read());
                        next.count++;

                    } else if (next.count == 1) {
                        next.command.size = (uint8_t) (this->stream->read());
                        next.count++;
                    }

                } else {
                    next.stage++;
                    next.count = 0;
                }

                //If there is still stuff in the stream buffer to read and we are on stage 2, go ahead and fall into it
                if (next.stage == 1 || !this->stream->available()) {
                    break;
                }
            }
            case 2: {
                //Read available chunks into the buffer
                next.count += this->stream->readBytes(CommandProtocol::default_buffer + next.count,
                                                      min(next.command.size - next.count,
                                                          (uint8_t) this->stream->available()));

                if (next.count == next.command.size) {
                    next.stage++;
                }

                break;
            }
            case 4: {
                //only memset what has not been read into
                auto diff = COMMAND_PROTOCOL_DEFAULT_BUFFER_SIZE - next.command.size;

                //do memset
                memset(CommandProtocol::default_buffer + next.command.size, 0, diff);

                //assign default buffer to next command data
                //if the action want to keep the data persistent,
                //  it should copy it into some other structure
                next.command.data = CommandProtocol::default_buffer;

                next.stage++;

                break;
            }
            case 3: {
                next.waiting = false;

                (*this->callback)(&next.command);

                next.stage = 0;
            }
        }
    }
}

bool CommandProtocol::sendCommand(const Command &request, callback_t callback) {
    this->setCallback(callback);
    return this->sendCommand(request);
}

void CommandProtocol::setCallback(CommandProtocol::callback_t callback) {
    this->callback = callback;
}

CommandProtocol::callback_t CommandProtocol::getCallback() const {
    return this->callback;
}

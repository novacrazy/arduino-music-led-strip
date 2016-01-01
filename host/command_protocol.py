import serial
import time
import struct

import commands

START_MESSAGE = "Hello, World!"
MAGIC_HEADER = 0xFF

COMMAND_PROTOCOL_DEFAULT_BUFFER_SIZE = 256
COMMAND_PROTOCOL_DEFAULT_WAIT_TIME = 0.1


def default_fmt(data):
    msg = b''

    for i in data:
        msg += struct.pack("<B", i)

    return msg


class Command:
    def __init__(self, cmd, data=None, fmt=default_fmt):
        self.cmd = cmd
        self.data = data
        self.fmt = fmt


class CommandProtocolMagic:
    def __init__(self, start_message=START_MESSAGE, magic_header=MAGIC_HEADER):
        self.start_message = start_message
        self.magic_header = magic_header


class NextCommand:
    def __init__(self):
        self.stage = 0
        self.count = 0
        self.command = Command(commands.COMMAND_NO_OP)
        self.waiting = False
        self.callback = None


class CommandProtocol:
    def __init__(self, stream, magic=CommandProtocolMagic()):
        """
        :param serial.Serial stream:
        :param CommandProtocolMagic magic:
        """
        self.stream = stream
        self.magic = magic
        self.initialized = False
        self.next = NextCommand()

    def start(self):
        if not self.initialized:
            stage = 0
            it = 0
            length = len(self.magic.start_message)

            do_loop = True

            while do_loop:
                if stage is 0:
                    self.stream.write(self.magic.start_message)
                    stage += 1
                elif stage is 1:
                    while self.stream.in_waiting:
                        next = self.stream.read()

                        if self.magic.start_message[it] == next:
                            it += 1

                            if it == length:
                                stage += 1
                                break

                        else:
                            it = 0

                elif stage is 2:
                    self.initialized = True
                    do_loop = False

                time.sleep(COMMAND_PROTOCOL_DEFAULT_WAIT_TIME)

    def is_waiting(self):
        return self.next.waiting

    def send_request(self, request, callback):
        """
        :param Command request:
        :param Function callback:
        :rtype: bool
        """
        if self.is_waiting():
            return False

        else:
            sent = 0
            length = 0
            formatted = b""

            if request.data is not None:
                formatted = request.fmt(request.data)

                length = len(formatted)

                if length > COMMAND_PROTOCOL_DEFAULT_BUFFER_SIZE:
                    return False

            sent += self.stream.write(self.magic.magic_header)

            sent += self.stream.write(struct.pack("<BB", request.cmd, length))

            if length > 0:
                sent += self.stream.write(formatted)

            if sent == length + 3:
                self.next.callback = callback
                self.next.waiting = True

                return True

            else:
                return False

    def yield_stream(self):
        pass
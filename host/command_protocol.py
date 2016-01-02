import serial
import time
import struct

import commands

START_MESSAGE = "Hello, World!"
MAGIC_HEADER = '\xff'

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
        self.size = 0

    def __repr__(self):
        return "Command(cmd=%x, size=%d)" % (self.cmd, self.size)

    def __str__(self):
        return "member of Command(cmd=%x, size=%d)" % (self.cmd, self.size)


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

    def start(self, callback=None):
        n = self.next
        s = self.stream
        m = self.magic

        if not self.initialized:
            stage = 0
            it = 0
            length = len(m.start_message)

            while True:
                if stage is 0:
                    s.write(m.start_message)
                    stage += 1

                elif stage is 1:
                    while s.in_waiting:
                        b = s.read()

                        if m.start_message[it] == b:
                            it += 1

                            if it == length:
                                stage += 1
                                break

                        else:
                            it = 0

                elif stage is 2:
                    self.initialized = True
                    break

                time.sleep(COMMAND_PROTOCOL_DEFAULT_WAIT_TIME)

            if callback is not None:
                n.callback = callback

    def is_waiting(self):
        return self.next.waiting

    def send_command(self, request, callback=None):
        """
        :param Command request:
        :param Function callback:
        :rtype: bool
        """
        n = self.next
        s = self.stream

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

            sent += s.write(self.magic.magic_header)

            sent += s.write(struct.pack("<BB", request.cmd, length))

            if length > 0:
                sent += s.write(formatted)

            if sent == length + 3:
                self.await_command(callback)

                return True

            else:
                return False

    def await_command(self, callback=None):
        self.next.waiting = True
        if callback is not None:
            self.next.callback = callback

    def run(self):
        n = self.next
        s = self.stream

        if self.is_waiting() and s.in_waiting:
            if n.stage is 0:
                c = s.read()

                if c == self.magic.magic_header:
                    n.stage += 1

                n.count = 0

            if n.stage is 1:
                print("Reading header")
                n.command.cmd, n.command.size = struct.unpack("<BB", s.read(2))

                n.stage += 1

            # if there is still data to be read, go ahead and try to read it
            if n.stage is 2 and s.in_waiting:
                print("Reading data")
                n.command.data = s.read(n.command.size)

                n.stage += 1

            elif n.stage is 2 and n.command.size is 0:
                n.stage += 1

            # This is for strict compatibility with the C++ protocol stages
            if n.stage is 3:
                n.stage += 1

            if n.stage is 4:
                print("Calling callback")
                n.waiting = False
                if n.callback is not None:
                    n.callback(n.command)
                n.stage = 0


if __name__ == '__main__':
    def my_callback(cmd):
        """
        :param Command cmd:
        """
        if cmd.cmd is commands.COMMAND_REQUEST_ACTION:
            print("HERE")


    ser = serial.Serial("COM5", 9600)

    time.sleep(1)

    protocol = CommandProtocol(ser)

    protocol.start(my_callback)

    print("Initialized")

    protocol.await_command()

    while True:
        protocol.run()
        time.sleep(0.01)

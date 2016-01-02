"""PyAudio example: Record a few seconds of audio and save to a WAVE file."""

from listen_thread import ListenerThread
import commands
from command_protocol import CommandProtocol, Command

import serial
import time
import struct
import numpy as np
import numpy.fft as fft
from collections import deque

m = ListenerThread()

m.start()

lim = (1 << 13)
limn = -lim - 1

bands = ((0, 60),  # 255, 0,   0
         (60, 250),  # 255, 171, 0
         (250, 500),  # 100, 255, 0
         (500, 1000),  # 50,  255, 150
         (1000, 4000),  # 0,   255, 255
         (4000, 10000),  # 0,   100, 255
         (10000, 20000))  # 0,   0,   255


def map_range(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min


zeroed = False
last_time = time.time()

refresh_rate = 1.0 / 60.0

peaks = deque(maxlen=(100 * 10))  # at 100 requests per second, this would be 10 seconds

ser = serial.Serial('COM5', 256000, parity=serial.PARITY_EVEN, stopbits=serial.STOPBITS_TWO)

time.sleep(2)

protocol = CommandProtocol(ser)


def format_stereo(data):
    return struct.pack("<fff", *data)


def write_stereo(left, right):
    global protocol, peaks
    stereo_cmd = Command(cmd=commands.COMMAND_WRITE_STEREO, data=(left, right, np.max(peaks)), fmt=format_stereo)

    res = protocol.send_command(stereo_cmd)


def callback(cmd):
    global zeroed, peaks, last_time, protocol

    if cmd.cmd is commands.COMMAND_REQUEST_ACTION:
        channels = np.fromstring(''.join(m.data), dtype=np.int16).reshape((2, -1), order='F')

        if channels.any():
            mono = channels.mean(axis=0)

            avg_level = np.absolute(mono).mean()

            if avg_level >= 1.0:
                mean = np.absolute(channels).mean(axis=1)

                peaks.append(mean.max())

                # print(len(mono))

                # w = fft.fft(mono)
                # freqs = fft.fftfreq(len(w))

                # print(len(w))

                # print(len(freqs))

                # idx = np.argmax(np.abs(w))
                # freq = freqs[idx]
                # freq_in_hertz = abs(freq * 44100)
                # print(freq_in_hertz)

                # ranges = []
                # current_band = 0

                # for freq in freqs:
                #     if freq >

                # plt.plot(freqs, w.real)

                # value = np.sum((min, max), axis=0).astype(np.uint8)

                write_stereo(mean[0], mean[1])

                zeroed = False
                last_time = time.time()

            elif time.time() - last_time > 15.0:
                time.sleep(0.5)
                write_stereo(0, 0)
            else:
                write_stereo(0, 0)
        else:
            write_stereo(0, 0)


protocol.start(callback)

print("Initialized")

protocol.await_command()

while True:
    protocol.run()
    time.sleep(1.0 / 100.0)

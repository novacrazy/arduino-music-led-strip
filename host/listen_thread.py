# -*- coding: utf-8 -*-

from threading import Thread
import pyaudio
import numpy as np
from collections import deque

CHUNK = 256
CHANNELS = 2
RATE = 44100

MAX_BUFFER_SIZE = 2048 * 4


class ListenerThread(Thread):
    def __init__(self):
        Thread.__init__(self)

        self.p = pyaudio.PyAudio()
        self.closed = True
        self.data = deque(maxlen=MAX_BUFFER_SIZE)
        self.stream = None

    def run(self):
        device = self.find_device("What You Hear (Sound Blaster Z)")

        def callback(in_data, frame_count, time_info, status):
            self.data.extend(in_data)
            return (in_data, pyaudio.paContinue)

        stream = self.stream = self.p.open(format=pyaudio.paInt16,
                                      channels=CHANNELS,
                                      rate=RATE,
                                      input=True,
                                      output=False,
                                      input_device_index=device,
                                      stream_callback=callback,
                                      frames_per_buffer=CHUNK)

        stream.start_stream()

        self.closed = False

    def join(self, timeout=None):
        if not self.closed:
            self.closed = True
            self.stream.stop_stream()
            self.stream.close()

        Thread.join(self, timeout)

    def find_device(self, name):
        for i in range(0, self.p.get_device_count()):
            device = self.p.get_device_info_by_index(i)
            if name in device['name']:
                return i
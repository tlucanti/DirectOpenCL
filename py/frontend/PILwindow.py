
import os
import threading
import time
import sys

from PIL import Image
import numpy as np

from .GUIwindow import GUIwindow
from .util.getch import getch
from .util.getch import Tracker

class PILwindow(GUIwindow):

    ENV_NAME = 'GUILIB_PILIMAGE_FNAME'

    def __init__(self, width, height, winid=None):
        if winid is None:
            winid = self
        self.__width = width
        self.__height = height
        self.__winid = winid
        self.__callback = None
        self.__pressed = False
        self.__tracker = Tracker(self.__key_handler)
        self.__listener_thread = threading.Thread(target=self.__key_listener)
        self.__listener_thread.start()

    def width(self):
        return self.__width

    def height(self):
        return self.__height

    def draw(self, image):
        assert image.shape == (self.__height, self.__width, 3)

        name = os.environ.get(self.ENV_NAME, 'frame.png')
        img = Image.fromarray(np.uint8(image))
        img.save(name)

    def key_hook(self, callback):
        self.__callback = callback

    def __key_handler(self, key, pressed):
        self.__pressed = True
        if self.__callback is not None:
            #print(key, pressed)
            self.__callback(self.__winid, key, pressed)


    def wfi(self):
        self.__pressed = False
        while not self.__pressed:
            time.sleep(1e-5)

    def mouse(self):
        return 0, 0

    def __key_listener(self):
        while True:
            key = getch()
            print(f'pressed {key}')
            if key == ord('x'):
                sys.exit()
            self.__tracker.press(key)

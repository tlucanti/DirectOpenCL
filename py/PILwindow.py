
import os
import threading

from PIL import Image
import numpy as np

from GUIwindow import GUIwindow

class PILwindow(GUIwindow):

    ENV_NAME = 'GUILIB_PILIMAGE_FNAME'

    def __init__(self, width, height):
        self.__width = width
        self.__height = height
        self.__callback = None
        self.__listener_thread = threading.Thread(target=self.__key_listener)
        self.__listener_thread.start()

    def width(self):
        return self.__width

    def height(self):
        return self.__height

    def draw(self, image):
        assert image.shape == (self.__height, self.__width, 3)

        name = os.environ.get(self.ENV_NAME, 'frame.jpg')
        img = Image.fromarray(np.uint8(image))
        img.save(name)

    def key_hook(self, callback):
        self.__callback = callback

    def mouse(self):
        pass

    def __key_listener(self):
        while True:
            key = input('press key to move >> ')
            for k in key:
                if self.__callback is not None:
                    self.__callback(ord(k), True)
                    self.__callback(ord(k), False)



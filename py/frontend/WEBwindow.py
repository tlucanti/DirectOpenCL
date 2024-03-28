
import pickle
import numpy as np
import threading
import time

from .util.NetSock import Server

class WebWindow():
    def __init__(self, width, height, winid=None):
        self.__width = width
        self.__height = height
        if winid is None:
            winid = self
        self.__winid = winid
        self.__callback = None
        self.__waiting_for_mouse = False
        self.__waiting_for_interrupt = False
        self.__mouse_x = 0
        self.__mouse_y = 0

        self.__server = Server()
        print('server: waiting for client')
        self.__server.accept()

        self.__server.send_string('R')
        self.__server.send_number(width)
        self.__server.send_number(height)

        self.__thread_obj = threading.Thread(target=self.__key_reader_thread)
        self.__thread_obj.start()

    def width(self):
        return self.__width

    def height(self):
        return self.__height

    def draw(self, image):
        data = pickle.dumps(image)
        self.__server.send_string('p')
        self.__server.send_number(len(data))
        self.__server.do_send(data)

    def key_hook(self, callback):
        self.__callback = callback

    def mouse(self):
        self.__waiting_for_mouse = True
        self.__server.send_string('m')
        while self.__waiting_for_mouse:
            time.sleep(1e-5)

        return self.__mouse_x, self.__mouse_y

    def wfi(self):
        self.__waiting_for_interrupt = True
        while self.__waiting_for_interrupt:
            time.sleep(1e-5)
        return

    def __key_reader_thread(self):
        while True:
            event = self.__server.recv_string(1)

            if event == 'K' or event == 'k':
                pressed = (event == 'K')
                k = self.__server.recv_number()
                print(f'server: key {k} {"pressed" if pressed else "released"}')
                if self.__callback:
                    self.__callback(self.__winid, k, pressed)
            elif event == 'M':
                self.__mouse_x = self.__server.recv_number()
                self.__mouse_y = self.__server.recv_number()
                print(f'server: mouse at {self.__mouse_x}:{self.__mouse_y}')
                self.__waiting_for_mouse = False
            else:
                print(f'server: unknown event: {event}')

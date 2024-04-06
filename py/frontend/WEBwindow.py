
import pickle
import numpy as np
import threading
import time
import cv2

from .util.NetSock import TcpServer, UdpServer

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

        print('server: waiting for client')
        self.pix_socket = TcpServer(port=7778)
        self.event_socket = TcpServer(port=7777)

        self.event_socket.send('R', width, height)
        self.event_socket.send('E', 'P')

        self.__thread_obj = threading.Thread(target=self.__key_reader_thread)
        self.__thread_obj.start()

    def width(self):
        return self.__width

    def height(self):
        return self.__height

    def draw(self, image):
        frame = cv2.imencode('.jpg', image, (cv2.IMWRITE_JPEG_QUALITY, 100))[1].tobytes()
        self.pix_socket.send(len(frame), frame)

    def key_hook(self, callback):
        self.__callback = callback

    def mouse(self):
        self.__waiting_for_mouse = True
        self.event_socket.send('m')
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
            event = self.event_socket.recv_string(1)

            if event == 'K' or event == 'k':
                pressed = (event == 'K')
                k = self.event_socket.recv_number()
                # print(f'server: key {k} {"pressed" if pressed else "released"}')
                if self.__callback:
                    self.__callback(self.__winid, k, pressed)
            elif event == 'M':
                self.__mouse_x = self.event_socket.recv_number()
                self.__mouse_y = self.event_socket.recv_number()
                # print(f'server: mouse at {self.__mouse_x}:{self.__mouse_y}')
                self.__waiting_for_mouse = False
            else:
                print(f'server: unknown event: {event}')

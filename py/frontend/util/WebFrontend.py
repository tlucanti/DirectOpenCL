
import numpy as np
import time
import threading
import cv2

from .NetSock import TcpClient, UdpClient

def time_report(names, times):
    assert len(names) + 1 == len(times)

    total = times[-1] - times[0]
    for i in range(len(names)):
        delta = times[i + 1] - times[i]
        percent = 0 if total < 1e-8 else delta / total * 100
        fps = float("inf") if delta < 1e-8 else 1.0 / delta
        print(f'{names[i]}:\t{round(delta * 1000)}ms\t({percent:.1f}%)', end='')
        print(f'\tpossible fps: {fps:.1f}')
    print(f'total:\t {round(total * 1000)}ms (fps: {1 / total:.1f})')
    print()


class WebFrontend():
    def __init__(self, constructor, host, port=7777):
        pix_socket = TcpClient(host, port + 1)
        self.__event_socket = TcpClient(host, port)

        event = self.__event_socket.recv_char()
        assert event == 'R'
        self.__width = self.__event_socket.recv_number()
        self.__height = self.__event_socket.recv_number()

        event = self.__event_socket.recv_char()
        assert event == 'E'
        self.__encoding = self.__event_socket.recv_char()
        if self.__encoding == 'B':
            self.__encoding += self.__event_socket.recv_char()

        self.__frontend = constructor(self.__width, self.__height)
        self.__frontend.key_hook(self.__key_callback)

        threading.Thread(target=self.__draw_thread, args=[pix_socket]).start()

        while True:
            event = self.__event_socket.recv_char()
            if event == 'm':
                x, y = self.__frontend.mouse()
                self.__event_socket.send('M', x, y)
            else:
                print(f'client: unknown event {event}')

    def __draw_thread(self, pix_socket):
        if self.__encoding == 'B1':
            while True:
                start = time.time()

                size = pix_socket.recv_number()
                data = pix_socket.do_recv(size)
                assert len(data) == size
                recv = time.time()

                data = np.frombuffer(data, np.uint8)
                img = cv2.imdecode(data, cv2.IMREAD_COLOR)
                decode = time.time()

                self.__frontend.draw(img)
                draw = time.time()

                time_report(['recv', 'decode', 'draw'], [start, recv, decode, draw])

        elif self.__encoding == 'B0':
            while True:
                start = time.time()

                size = pix_socket.recv_number()
                data = pix_socket.do_recv(size)
                assert len(data) == size
                recv = time.time()

                array = np.frombuffer(data, dtype=np.uint8)
                array = array.reshape((self.__height, self.__width, 3))
                decode = time.time()

                self.__frontend.draw(array)
                draw = time.time()

                time_report(['recv', 'decode', 'draw'], [start, recv, decode, draw])

    def __key_callback(self, winid, keycode, pressed):
        event = 'K' if pressed else 'k'
        self.__event_socket.send(event, keycode)

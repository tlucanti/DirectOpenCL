
import numpy as np
import pickle
import time
import threading

from .NetSock import TcpClient

def time_report(names, times):
    assert len(names) + 1 == len(times)

    total = times[-1] - times[0]
    for i in range(len(names)):
        delta = times[i + 1] - times[i]
        print(f'{names[i]}:\t{round(delta * 1000)}ms\t({delta / total * 100:.1f}%)\tpossible fps: {float("inf") if delta == 0 else 1.0 / delta:.1f}')
    print(f'total:\t {round(total * 1000)}ms (fps: {1 / total:.1f})')
    print()


class WebFrontend():
    def __init__(self, constructor, host, port=7777):
        self.__event_socket = TcpClient(host, port)
        pix_socket = TcpClient(host, port + 1)

        event = self.__event_socket.recv_char()
        assert event == 'R'
        self.__width = self.__event_socket.recv_number()
        self.__height = self.__event_socket.recv_number()

        self.__frontend = constructor(self.__width, self.__height)
        self.__frontend.key_hook(self.__key_callback)

        threading.Thread(target=self.__draw_thread, args=[pix_socket]).start()

        while True:
            event = self.__event_socket.recv_char()
            if event == 'm':
                x, y = self.__frontend.mouse()
                self.__event_socket.send_string('M')
                self.__event_socket.send_number(x)
                self.__event_socket.send_number(y)
            else:
                print(f'client: unknown event {event}')

    def __draw_thread(self, pix_socket):
        encoding = pix_socket.recv_char()
        if encoding == 'B':
            encoding += pix_socket.recv_char()

        if encoding == 'P':
            size = pix_socket.recv_number()
            while True:
                start = time.time()

                data = pix_socket.do_recv(size)
                assert len(data) == size
                recv = time.time()

                data = pickle.loads(data)
                decode = time.time()

                self.__frontend.draw(data)
                draw = time.time()

                time_report(['recv', 'decode', 'draw'], [start, recv, decode, draw])

        elif encoding == 'B0':
            size = pix_socket.recv_number()
            while True:
                start = time.time()

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
        self.__event_socket.send_string('K' if pressed else 'k')
        self.__event_socket.send_number(keycode)

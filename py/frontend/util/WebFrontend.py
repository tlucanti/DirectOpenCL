
import numpy as np
import time
import threading
import io

from PIL import Image

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

        self.__mouse = None
        while True:
            xy = self.__frontend.mouse()
            if self.__mouse != xy:
                self.__mouse = xy
                self.__event_socket.send('M', xy[0], xy[1])
            time.sleep(5e-2)

    def __to_numpy(self, im, buffer):
        im.load()
        # unpack data
        e = Image._getencoder(im.mode, 'raw', im.mode)
        e.setimage(im.im)

        mem = buffer.data.cast('B', (buffer.data.nbytes,))

        bufsize, s, offset = 65536, 0, 0
        while not s:
            l, s, d = e.encode(bufsize)
            mem[offset:offset + len(d)] = d
            offset += len(d)
        if s < 0:
            raise RuntimeError("encoder error %d in tobytes" % s)

    def __draw_thread(self, pix_socket):
        frames = 0
        if self.__encoding == 'B1':
            buffer = np.empty((self.__height, self.__width, 3), dtype=np.uint8)
            while True:
                frames += 1
                start = time.time()

                size = pix_socket.recv_number()
                data = pix_socket.do_recv(size)
                assert len(data) == size
                recv = time.time()
                if frames % 2:
                    continue

                img = Image.open(io.BytesIO(data))
                decode = time.time()

                self.__to_numpy(img, buffer)
                alloc = time.time()

                self.__frontend.draw(buffer)
                draw = time.time()

                time_report(['recv', 'decode', 'alloc', 'draw'], [start, recv, decode, alloc, draw])
                print('frame', frames)
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
        print('SEND', event, keycode)
        self.__event_socket.send(event, keycode)

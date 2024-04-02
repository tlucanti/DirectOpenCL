
import numpy as np
import pickle
import time

from .NetSock import Client

def time_report(names, times):
    assert len(names) + 1 == len(times)

    total = times[-1] - times[0]
    for i in range(len(names)):
        delta = times[i + 1] - times[i]
        print(f'{names[i]}:\t{round(delta * 1000)}ms\t({delta / total * 100:.1f}%)')
    print(f'total:\t {round(total * 1000)}ms (fps: {1 / total:.1f})')
    print()


class WebFrontend():
    def __init__(self, constructor, host, port=7777):
        self.__client = Client(host, port)

        event = self.__client.recv_string(1)
        assert event == 'R'
        width = self.__client.recv_number()
        height = self.__client.recv_number()

        self.__frontend = constructor(width, height)
        self.__frontend.key_hook(self.__key_callback)

        while True:
            start = time.time()
            event = self.__client.recv_string(1)
            if event == 'P':
                size = self.__client.recv_number()
                # print(f'client: getting pickle of size {size}')
                prep = time.time()

                data = self.__client.do_recv(size)
                recv = time.time()

                # print(f'client: got pickle of size {len(data)}')
                data = pickle.loads(data)
                decode = time.time()

                # print(f'client: got array {data}')
                self.__frontend.draw(data)
                draw = time.time()

                time_report(['prep', 'recv', 'decode', 'draw'], [start, prep, recv, decode, draw])

            elif event == 'B':
                compression = self.__client.recv_string(1)

                if compression   == '0':
                    size = self.__client.recv_number()
                    prep = time.time()

                    data = self.__client.do_recv(size)
                    recv = time.time()

                    width, height = self.__frontend.width(), self.__frontend.height()
                    recv = time.time()

                    assert len(data) == size
                    assert size == width * height * 3

                    array = np.frombuffer(data, dtype=np.uint8)
                    array = array.reshape((height, width, 3))
                    decode = time.time()

                    self.__frontend.draw(array)
                    draw = time.time()

                time_report(['prep', 'recv', 'decode', 'draw'], [start, prep, recv, decode, draw])
            elif event == 'm':
                x, y = self.__frontend.mouse()
                self.__client.send_string('M')
                self.__client.send_number(x)
                self.__client.send_number(y)
            else:
                print(f'client: unknown event {event}')


    def __key_callback(self, winid, keycode, pressed):
        self.__client.send_string('K' if pressed else 'k')
        self.__client.send_number(keycode)

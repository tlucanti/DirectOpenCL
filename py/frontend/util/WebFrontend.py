
import numpy as np
import pickle

from .NetSock import Client

class WebFrontend():
    def __init__(self, constructor):
        self.__client = Client()

        event = self.__client.recv_string(1)
        assert event == 'R'
        width = self.__client.recv_number()
        height = self.__client.recv_number()

        self.__frontend = constructor(width, height)
        self.__frontend.key_hook(self.__key_callback)

        while True:
            event = self.__client.recv_string(1)
            if event == 'p':
                size = self.__client.recv_number()
                # print(f'client: getting pickle of size {size}')
                data = self.__client.do_recv(size)
                # print(f'client: got pickle of size {len(data)}')
                data = pickle.loads(data)
                # print(f'client: got array {data}')
                self.__frontend.draw(data)
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

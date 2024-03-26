
from .client import Client
import numpy as np
import pickle

class WEBwindow():
    def __init__(self, frontend):
        self.__frontend = frontend
        self.__client = Client()
        self.__frontend.key_hook(self.__key_callback)

        while True:
            data = self.__client.recv()
            data = pickle.loads(data)
            print(f'got array {data}')


    def __key_callback(self, winid, keycode, pressed):
        if pressed:
            self.__client.send(f'K;{keycode}'.encode('utf-8'))
        else:
            self.__client.send(f'k;{keycode}'.encode('utf-8'))

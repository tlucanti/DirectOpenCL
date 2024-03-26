
import socket
import pickle
import numpy as np

class Server():

    HOST = '127.0.0.1'

    def __init__(self, port=7777):
        self.soc = socket.create_server((self.HOST, port), reuse_port=False)

    def accept(self):
        self.conn, addr = self.soc.accept()
        print(f'Connected by {addr}')

    def recv(self):
        data = self.conn.recv(10000)
        if not data:
            raise ConnectionResetError('client closed connection')
        return data

    def send(self, data):
        self.conn.send(data)


if __name__ == '__main__':
    se = Server()
    se.accept()
    a = np.ones(2)

    while True:
        data = se.recv().decode('utf-8')
        if data[0] == 'K':
            k = data.split(';')[1]
            print(f'key {k} pressed')
        elif data[0] == 'k':
            k = data.split(';')[1]
            print(f'key {k} released')
        else:
            print(f'unknown data: {data}')

        data = pickle.dumps(a)
        print('sending array')
        se.send(data)

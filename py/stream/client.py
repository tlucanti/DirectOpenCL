
import socket

class Client():
    def __init__(self, host='127.0.0.1', port=7777):
        self.soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.soc.connect((host, port))

    def __del__(self):
        self.soc.close()

    def send(self, data):
        self.soc.sendall(data)

    def recv(self):
        data = self.soc.recv(10000)
        if not data:
            raise ConnectionResetError('client closed connection')
        return data


if __name__ == '__main__':
    cl = Client()

    print('client: sending Interrupt')
    cl.send('k;123')
    while True:
        cl.send(input())

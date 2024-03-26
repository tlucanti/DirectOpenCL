
import socket

class Server():

    HOST = '127.0.0.1'

    def __init__(self, port=7777):
        self.soc = socket.create_server((self.HOST, port), reuse_port=True)

    def accept(self):
        self.conn, addr = self.soc.accept()
        print(f'Connected by {addr}')

    def recv(self):
        data = self.conn.recv(1024).decode('utf-8')
        if not data:
            raise ConnectionResetError('client closed connection')
        return data


if __name__ == '__main__':
    se = Server()
    se.accept()

    while True:
        print(se.recv())



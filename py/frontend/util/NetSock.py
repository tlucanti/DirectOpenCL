
import socket

class NetSock():
	def __init__(self):
		pass

	def do_send(self, bytes):
		pass

	def do_recv(self, n):
		pass

	def send_string(self, data):
		self.do_send(str(data).encode('utf-8'))

	def send_number(self, num):
		self.send_string(f'{num:+010}')

	def recv_string(self, n=9999):
		return self.do_recv(n).decode('utf-8')

	def recv_number(self):
		return int(self.recv_string(10))


class Server(NetSock):

    HOST = '127.0.0.1'

    def __init__(self, port=7777):
        self.soc = socket.create_server((self.HOST, port), reuse_port=False)

    def __del__(self):
        self.soc.close()

    def accept(self):
        self.conn, addr = self.soc.accept()
        print(f'Connected by {addr}')

    def do_send(self, bytes):
        self.conn.send(bytes)

    def do_recv(self, n):
        data = self.conn.recv(n)
        if not data:
            raise ConnectionResetError('client closed connection')
        return data


class Client(NetSock):
    def __init__(self, host='127.0.0.1', port=7777):
        self.soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.soc.connect((host, port))

    def __del__(self):
        self.soc.close()

    def do_send(self, bytes):
        self.soc.sendall(bytes)

    def do_recv(self, n):
        data = self.soc.recv(n)
        if not data:
            raise ConnectionResetError('server closed connection')
        return data

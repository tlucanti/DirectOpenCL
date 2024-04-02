
import socket

class NetSock():
	def __init__(self):
		self.fd = None

	def do_send(self, bytes):
		self.fd.send(bytes)

	def do_recv(self, n):
		ret = b''
		while len(ret) < n:
			data = self.fd.recv(n - len(ret))
			if not data:
				raise ConnectionResetError('client closed connection')
			ret += data
		return ret

	def send_string(self, data):
		self.do_send(str(data).encode('utf-8'))

	def send_number(self, num):
		num = f'{num:+010}'
		assert len(num) == 10
		self.send_string(num)

	def recv_string(self, n):
		return self.do_recv(n).decode('utf-8')

	def recv_number(self):
		return int(self.recv_string(10))


class Server(NetSock):
	def __init__(self, host='0.0.0.0', port=7777):
		print(f'running server at {host}:{port}')
		self.soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.soc.bind((host, port))
		self.soc.listen(1)

	def __del__(self):
		self.soc.close()

	def accept(self):
		conn, addr = self.soc.accept()
		self.fd = conn
		print(f'Connected by {addr}')


class Client(NetSock):
	def __init__(self, host, port=7777):
		self.fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.fd.connect((host, port))

	def __del__(self):
		self.fd.close()


import socket

class TcpNetSock():
	def __init__(self, fd):
		self.fd = fd
		self.buf = b''

	def do_send(self, bytes):
		self.fd.send(bytes)

	def do_recv(self, n):
		ret = b''

		while n > len(self.buf):
			ret += self.buf
			n -= len(self.buf)

			self.buf = self.fd.recv(max(1024, n))
			if (len(self.buf) < 1024):
				print('BUFF LEN', len(self.buf))
			if not self.buf:
				raise ConnectionResetError('client closed connection')

		ret += self.buf[:n]
		self.buf = self.buf[n:]

		return ret

	def send_string(self, data):
		self.do_send(str(data).encode())

	def send_char(self, char):
		self.do_send(char.encode())

	def send_number(self, num):
		num = f'{num:+010}'
		assert len(num) == 10
		self.send_string(num)

	def recv_string(self, n):
		return self.do_recv(n).decode()

	def recv_char(self):
		return chr(self.do_recv(1)[0])

	def recv_number(self):
		return int(self.recv_string(10))


class TcpServer(TcpNetSock):
	def __init__(self, host='0.0.0.0', port=7777):
		print(f'running server at {host}:{port}')
		self.soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.soc.bind((host, port))
		self.soc.listen(1)

		conn, addr = self.soc.accept()
		print(f'Connected by {addr}')

		super().__init__(conn)


	def __del__(self):
		self.soc.close()


class TcpClient(TcpNetSock):
	def __init__(self, host, port=7777):
		self.fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.fd.connect((host, port))

		super().__init__(self.fd)

	def __del__(self):
		self.fd.close()

Server = TcpServer
Client = TcpClient

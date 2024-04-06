
import socket

class TcpNetSock():
	def __init__(self, fd):
		self.fd = fd
		self.buf = b''

	def do_recv(self, n):
		ret = b''

		while n > len(self.buf):
			ret += self.buf
			n -= len(self.buf)

			self.buf = self.fd.recv(max(1024, n))
			if not self.buf:
				raise ConnectionResetError('client closed connection')

		ret += self.buf[:n]
		self.buf = self.buf[n:]

		return ret

	def send(self, *args):
		packet = b''
		for a in args:
			if isinstance(a, int):
				num = f'{a:+010}'
				assert len(num) == 10
				packet += num.encode()
			elif isinstance(a, str):
				packet += a.encode()
			elif isinstance(a, bytes):
				packet += a
			else:
				raise ValueError(f'cannot send a variable of type {type(a)}')

		self.fd.send(packet)

	def recv_string(self, n):
		return self.do_recv(n).decode()

	def recv_char(self):
		return chr(self.do_recv(1)[0])

	def recv_number(self):
		return int(self.recv_string(10))


class UdpNetSock():
	def __init__(self, fd, addr):
		self.fd = fd
		self.addr = addr

	def send(self, *args):
		packet = b''
		for a in args:
			if isinstance(a, int):
				num = f'{a:+010}'
				assert len(num) == 10
				packet += num.encode()
			elif isinstance(a, str):
				packet += a.encode()
			elif isinstance(a, bytes):
				packet += a
			else:
				raise ValueError(f'cannot send a variable of type {type(a)}')

		self.fd.sendto(packet, self.addr)

	def do_recv(self, n):
		return self.fd.recvfrom(n)[0]

	def recv_string(self, n):
		return self.do_recv(n).decode()

	def recv_char(self):
		return chr(self.do_recv(1)[0])

	def recv_number(self):
		return int(self.recv_string(10))


class TcpServer(TcpNetSock):
	def __init__(self, port, host='0.0.0.0'):
		print(f'running TCP server at {host}:{port}')
		self.soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.soc.bind((host, port))
		self.soc.listen(1)

		conn, addr = self.soc.accept()
		print(f'Connected by {addr}')

		super().__init__(conn)


	def __del__(self):
		self.soc.close()


class TcpClient(TcpNetSock):
	def __init__(self, host, port):
		print(f'connecting to {host}:{port}')
		self.fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.fd.connect((host, port))

		super().__init__(self.fd)

	def __del__(self):
		self.fd.close()


class UdpServer(UdpNetSock):
	def __init__(self, port, host='0.0.0.0'):
		print(f'running UDP server at {host}:{port}')
		self.fd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		# self.fd.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 1000000)
		self.fd.bind((host, port))
		_, addr = self.fd.recvfrom(0)

		super().__init__(self.fd, addr)

	def __del__(self):
		self.fd.close()


class UdpClient(UdpNetSock):
	def __init__(self, host, port):
		print(f'connecting to {host}:{port}')
		self.fd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		# self.fd.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 1000000)

		self.fd.sendto(b'', (host, port))

		super().__init__(self.fd, (host, port))

	def __del__(self):
		self.fd.close()

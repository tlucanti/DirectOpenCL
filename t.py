
import socket

s = socket.create_server(('0.0.0.0', 7777), reuse_port=True)
#s = socket.socket()
#s.bind(('0.0.0.0', 7777))
s.listen(1)
conn, addr = s.accept()
print(conn, addr)


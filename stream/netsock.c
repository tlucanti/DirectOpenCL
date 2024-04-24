
#include <guilib_internals.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <netsock.h>

static void print_soc_addr(int soc)
{
	struct sockaddr_in address;
	socklen_t size = sizeof(address);

	if (getpeername(soc, (struct sockaddr *)&address, &size) < 0) {
		gui_perror("getting client address fail");
		printf("Connected by UNKNOWN affress\n");
	} else {
		printf("Connected by %s\n", inet_ntoa(address.sin_addr));
	}
}

int soc_create_server(unsigned short port)
{
	const char *SERVER_HOST = "0.0.0.0";
	struct sockaddr_in address;
	unsigned int ip = inet_addr(SERVER_HOST);
	const int enable = 1;
	int soc;

	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(ip);

	soc = socket(AF_INET, SOCK_STREAM, 0);
	if (soc < 0) {
		gui_perror("create socket fail");
		goto fail;
	}

	if (setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		gui_perror("set socket option fail");
		goto fail;
	}

	if (bind(soc, (struct sockaddr *)&address, sizeof(address)) < 0) {
		gui_perror("socket bin fail");
		goto fail;
	}

	if (listen(soc, 1) < 0) {
		gui_perror("socket listen fail");
		goto fail;
	}

	printf("running server at %s:%hu\n", SERVER_HOST, port);

	return soc;

fail:
	close(soc);
	return -1;
}

int soc_server_accept(int soc, struct soc_stream *stream)
{
	struct sockaddr_in address;
	socklen_t size = sizeof(address);
	int client;

	client = accept(soc, (struct sockaddr *)&address, &size);
	if (client < 0) {
		gui_perror("client accept fail");
		return EFAULT;
	}

	print_soc_addr(client);

	stream->socket = client;
	stream->out_occupied = 0;
	return 0;
}

void soc_send(struct soc_stream *soc, const void *data, unsigned long size)
{
	ssize_t ret = 0;

	if (size + soc->out_occupied >= sizeof(soc->out_buff)) {
		soc_send_flush(soc);
	}
	if (size >= 1024) {
		ret = send(soc->socket, data, size, 0);
	} else {
		memcpy(soc->out_buff + soc->out_occupied, data, size);
		soc->out_occupied += size;
	}

	if (ret < 0) {
		gui_panic("send fail");
	}
}

void soc_recv(struct soc_stream *soc, void *dstp, unsigned long size)
{
	unsigned long got = 0;
	long rd;
	unsigned char *dst = dstp;

	while (got < size) {
		rd = recv(soc->socket, dst + got, size - got, 0);
		if (rd <= 0) {
			gui_panic("recv fail");
		}
		got += rd;
	}
}

void soc_send_string(struct soc_stream *soc, const char *s)
{
	soc_send(soc, s, strlen(s));
}

void soc_send_char(struct soc_stream *soc, char c)
{
	soc_send(soc, &c, 1);
}

void soc_send_number(struct soc_stream *soc, int number)
{
	char buf[12] = {};

	sprintf(buf, "%+010d", number);
	soc_send(soc, buf, 10);
}

void soc_send_flush(struct soc_stream *soc)
{
	if (soc->out_occupied == 0) {
		return;
	}

	send(soc->socket, soc->out_buff, soc->out_occupied, 0);
	soc->out_occupied = 0;
}

void soc_recv_string(struct soc_stream *soc, char *dst, unsigned long n)
{
	soc_recv(soc, dst, n);
}

char soc_recv_char(struct soc_stream *soc)
{
	char c;

	soc_recv(soc, &c, 1);
	return c;
}

int soc_recv_number(struct soc_stream *soc)
{
	char buf[12];

	soc_recv(soc, buf, 10);
	return atoi(buf);
}

void soc_close(int soc)
{
	if (close(soc)) {
		gui_panic("socket close fail");
	}
}



#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void print_soc_addr(int soc)
{
        struct sockaddr_in address;
        socklen_t size = sizeof(address);

        if (getpeername(soc, (struct sockaddr *)&address, &size) < 0) {
                fprintf(stderr, "print_soc_addr: getpeername() fail\n");
                abort();
        }

        printf("Connected by %s\n", inet_ntoa(address.sin_addr));
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
                fprintf(stderr, "soc_create_server: socket() fail\n");
                abort();
        }

        if (setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
                fprintf(stderr, "soc_create_server: setsockopt() fail\n");
                abort();
        }

        if (bind(soc, (struct sockaddr *)&address, sizeof(address)) < 0) {
                fprintf(stderr, "soc_create_server: bind() fail\n");
                abort();
        }

        if (listen(soc, 1) < 0) {
                fprintf(stderr, "soc_create_server: listen() fail\n");
                abort();
        }

        printf("running server at %s:%hu\n", SERVER_HOST, port);

        return soc;
}

int soc_server_accept(int soc)
{
        struct sockaddr_in address;
        socklen_t size = sizeof(address);
        int client;

        client = accept(soc, (struct sockaddr *)&address, &size);
        if (client < 0) {
                fprintf(stderr, "soc_server_accept: accept() fail\n");
                abort();
        }

        print_soc_addr(client);

        return client;
}

void soc_send(int soc, const void *data, unsigned long size)
{
        ssize_t ret = send(soc, data, size, 0);

        if (ret < 0) {
                fprintf(stderr, "soc_send: send() fail\n");
                abort();
        } else if (ret < (long)size) {
                fprintf(stderr, "soc_send: sent not all data\n");
                abort();
        }
}

void soc_recv(int soc, void *dstp, unsigned long size)
{
        unsigned long got = 0;
        long rd;
        unsigned char *dst = dstp;

        while (got < size) {
                rd = recv(soc, dst + got, size - got, 0);
                if (rd <= 0) {
                        fprintf(stderr, "soc_recv: recv() fail\n");
                        abort();
                }
                got += rd;
        }
}

void soc_send_string(int soc, const char *s)
{
        soc_send(soc, s, strlen(s));
}

void soc_send_char(int soc, char c)
{
        soc_send(soc, &c, 1);
}

void soc_send_number(int soc, int number)
{
        char buf[12] = {};

        sprintf(buf, "%+010d", number);
        soc_send(soc, buf, 10);
}

void soc_recv_string(int soc, char *dst, unsigned long n)
{
        soc_recv(soc, dst, n);
}
char soc_recv_char(int soc)
{
        char c;

        soc_recv(soc, &c, 1);
        return c;
}

int soc_recv_number(int soc)
{
        char buf[12];

        soc_recv(soc, buf, 10);
        return atoi(buf);
}

void soc_close(int soc)
{
        if (close(soc)) {
                fprintf(stderr, "soc_close: close() fail\n");
                abort();
        }
}


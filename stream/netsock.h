
#ifndef GUILIB_NETSOCK_H
#define GUILIB_NETSOCK_H

struct soc_stream {
        int socket;
        unsigned out_occupied;
        unsigned in_occupied;
        char out_buff[1024];
        char in_buff[1024];
};

int soc_create_server(unsigned short port);
void soc_server_accept(int soc, struct soc_stream *client);

void soc_send(struct soc_stream *soc, const void *data, unsigned long size);
void soc_send_string(struct soc_stream *soc, const char *s);
void soc_send_char(struct soc_stream *soc, char c);
void soc_send_number(struct soc_stream *soc, int number);
void soc_send_flush(struct soc_stream *soc);

void soc_recv(struct soc_stream *soc, void *dstp, unsigned long size);
void soc_recv_string(struct soc_stream *soc, char *dst, unsigned long n);
char soc_recv_char(struct soc_stream *soc);
int soc_recv_number(struct soc_stream *soc);
void soc_recv_flush(struct soc_stream *soc);

void soc_close(int soc);

#endif /* GUILIB_NETSOCK_H */


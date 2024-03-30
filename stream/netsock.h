
#ifndef GUILIB_NETSOCK_H
#define GUILIB_NETSOCK_H

int soc_create_server(unsigned short port);
int soc_server_accept(int soc);
void soc_send(int soc, const void *data, unsigned long size);
void soc_recv(int soc, void *dstp, unsigned long size);
void soc_send_string(int soc, const char *s);
void soc_send_char(int soc, char c);
void soc_send_number(int soc, int number);
void soc_recv_string(int soc, char *dst, unsigned long n);
char soc_recv_char(int soc);
int soc_recv_number(int soc);
void soc_close(int soc);

#endif /* GUILIB_NETSOCK_H */


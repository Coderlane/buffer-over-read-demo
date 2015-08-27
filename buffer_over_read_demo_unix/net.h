// Travis Lane
// Unix Socket Wrapper Header

#ifndef NET_H
#define NET_H

int net_init();
int net_cleanup();

int socket_connect(const char *hostname, const char *port, int *out_sock);
int socket_listen(const char *hostname, const char *port, int *out_sock);
int socket_accept(int listen_sock, int *out_sock);
int socket_close(int *sock);

int socket_send(int sock, const char *buff, int buff_len);
int socket_recv(int sock, char *buff, int buff_len);

#endif /* NET_H */

#ifndef SOCKET_HEADERS

#define SOCKET_HEADERS

int create_listening_socket(int *sockfd, const char *ip, const unsigned short port, const int backlog_size);
void close_socket(int *socket_fd);

#endif

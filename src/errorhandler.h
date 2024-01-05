#ifndef ERRORHANDLER_HEADERS

#define ERRORHANDLER_HEADERS

int handle_socket_err();
int handle_bind_err(const int sockfd);
int handle_listen_err();
int handle_accept_err();
int handle_recv_err();
int handle_close_err(const int sockfd);
int handle_connect_error();
int handle_send_error();

#endif

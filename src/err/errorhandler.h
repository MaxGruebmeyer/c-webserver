#ifndef ERRORHANDLER_HEADERS

#define ERRORHANDLER_HEADERS

/*
 * Header file containing methods for handling the multiple possible errors of the socket api.
 */

// TODO (GM): Move file error handling?
int handle_fopen_err();
int handle_fclose_err();
int handle_fread_err();
int handle_fseek_err();
int handle_ftell_err();

int handle_socket_err();
int handle_bind_err(const int sockfd);
int handle_listen_err();
int handle_accept_err();
int handle_recv_err();
int handle_close_err(const int sockfd);
int handle_connect_error();
int handle_send_error();

#endif

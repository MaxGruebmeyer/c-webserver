#include <stdio.h>
#include <errno.h>

#include "errorhandler.h"
#include "syscall.h"
#include "socket.h"

#define IP "127.0.0.1"
#define PORT 8080
#define BACKLOG_SIZE 100

#define MAX_MESSAGE_SIZE 1024

int main(void)
{
    /* AF_INET sockets can either be connection-oriented SOCK_STREAM
     * or connectionless SOCK_DGRAM, but not SOCK_SEQPACKET!
     */
    const int sockfd = socket(AF_INET, SOCK_STREAM);
    int connected_sockfd;

    char msg[MAX_MESSAGE_SIZE];
    struct sockaddr addr;

    if (sockfd == -1) {
        return handle_socket_err();
    }

    /* TODO (GM): Why does this not bind it to the correct port of 8080? */
    if (construct_sockaddr(&addr, sizeof(addr), IP, PORT) != 0) {
        printf("Could not construct server sockaddr %s:%i, error code %i!\n", IP, PORT, errno);
        return -1;
    }

    if (bind(sockfd, &addr, sizeof(addr)) != 0) {
        return handle_bind_err(sockfd);
    }

    if (listen(sockfd, BACKLOG_SIZE) != 0) {
        return handle_listen_err();
    }

    printf("Waiting for connections on %s:%i...\n", IP, PORT);

    if ((connected_sockfd = accept(sockfd)) == -1) {
        return handle_accept_err();
    }

    /* TODO (GM): Handle messages larger than MAX_MESSAGE_SIZE -> Set rcvbuf size somehow */
    /* TODO (GM): Set the MSG_DONTWAIT Flag to prevent blocking? */
    while (recvfrom(connected_sockfd, msg, MAX_MESSAGE_SIZE) != 0) {
        /* TODO (GM):
         * - Handle all possible error codes
         * - Build state machine that spins(?) if not connected
         */
        if (errno == ENOTCONN) {
            printf("Socket not connected!\n");
            continue;
        }

        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            handle_recv_err();
        }

        printf("Got an EAGAIN or EWOULDBLOCK, retrying...\n");
    }

    printf("Received a message: \"%s\"\n", msg);

    /* TODO (GM): Error handling -> Always close sockets during failure! */
    printf("Closing connected socket %i...\n", connected_sockfd);
    if (close(connected_sockfd) != 0) {
        handle_close_err(connected_sockfd);
    }

    printf("Closing socket %i...\n", sockfd);
    if (close(sockfd) != 0) {
        handle_close_err(sockfd);
    }

    printf("Done!\n");
    return 0;
}

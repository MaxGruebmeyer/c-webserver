#include <stdio.h>
#include <errno.h>

#include "errorhandler.h"
#include "syscall.h"
#include "socket.h"

#define IP "127.0.0.1"
#define PORT 8080
#define BACKLOG_SIZE 100

#define MAX_MESSAGE_SIZE 1024

static void close_socket(const int sockfd);

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

    printf("Trying to start server on %u.%u.%u.%u:%i\n",
            addr.sa_data.addr.ip[0], addr.sa_data.addr.ip[1], addr.sa_data.addr.ip[1], addr.sa_data.addr.ip[2],
            ((unsigned char)addr.sa_data.addr.port[0] << 8) + (unsigned char)addr.sa_data.addr.port[1]);

    if (bind(sockfd, &addr, sizeof(addr)) != 0) {
        close_socket(sockfd);
        return handle_bind_err(sockfd);
    }

    if (listen(sockfd, BACKLOG_SIZE) != 0) {
        close_socket(sockfd);
        return handle_listen_err();
    }

    printf("Watiting for connections...\n");

    if ((connected_sockfd = accept(sockfd)) == -1) {
        close_socket(sockfd);
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
        if (sendto(connected_sockfd, "HTTP Antwort", 13) == -1) {
            close_socket(connected_sockfd);
            close_socket(sockfd);

            return handle_send_error();
        }

        printf("Reply sent!\n");
    }

    printf("Received a message: \"%s\"\n", msg);

    close_socket(connected_sockfd);
    close_socket(sockfd);

    printf("Done!\n");
    return 0;
}

static void close_socket(const int sockfd)
{
    printf("Closing socket %i...\n", sockfd);
    if (close(sockfd) != 0) {
        handle_close_err(sockfd);
    }
}

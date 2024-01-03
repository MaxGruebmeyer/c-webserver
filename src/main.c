#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* TODO (GM): The following are not C std libraries - can they be replaced? */
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080
#define BACKLOG_SIZE 100

#define MAX_MESSAGE_SIZE 1024

static int handle_socket_err();
static int handle_bind_err(const int sockfd);
static int handle_listen_err();
static int handle_close_err(const int sockfd);

int main(void)
{
    /* AF_INET sockets can either be connection-oriented SOCK_STREAM
     * or connectionless SOCK_DGRAM, but not SOCK_SEQPACKET!
     */
    const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    char msg[MAX_MESSAGE_SIZE];
    struct sockaddr addr;

    if (sockfd == -1) {
        return handle_socket_err();
    }

    memset(&addr, 0, sizeof(addr));

    addr.sa_family = AF_INET;
    memcpy(addr.sa_data, "127.0.0.1", sizeof(addr.sa_data) - 1);

    printf("Addr: %s\n", addr.sa_data);

    if (bind(sockfd, &addr, sizeof(addr)) != 0) {
        return handle_bind_err(sockfd);
    }

    if (listen(sockfd, BACKLOG_SIZE) != 0) {
        return handle_listen_err();
    }

    /* TODO (GM): Handle messages larger than MAX_MESSAGE_SIZE -> Set rcvbuf size somehow */
    /* TODO (GM): Set the MSG_DONTWAIT Flag to prevent blocking? */
    recv(sockfd, msg, MAX_MESSAGE_SIZE, 0);
    printf("Received a message: %s\n", msg);

    if (close(sockfd) != 0) {
        return handle_close_err(sockfd);
    }

    return 0;
}

static int handle_socket_err()
{
    switch (errno) {
        case EACCES:
            printf("Could not create socket - permission for this type of socket or socket protocol denied!\n");
            break;
        case EAFNOSUPPORT:
            printf("Could not create socket - the implementation does not support the specified address family!\n");
            break;
        case EINVAL:
            printf("Could not create socket - unknown protocol, protocol family not available or invalid flags in type!\n");
            break;
        case EMFILE:
            printf("Could not create socket - the per-process limit on the number of open fds has been reached!\n");
            break;
        case ENFILE:
            printf("Could not create socket - the system-wide limit on the total number of open fds has been reached!\n");
            break;
        case ENOBUFS:
        case ENOMEM:
            printf("Could not create socket - insufficient memory!\n");
            break;
        case EPROTONOSUPPORT:
            printf("Could not create socket - the protocol or protocol type is not supported within this domain!\n");
            break;
        default:
            printf("Could not create socket - operation failed with error code %i!\n", errno);
            break;
    }

    return -1;
}

static int handle_bind_err(const int sockfd)
{
    switch (errno) {
        case EACCES:
            printf("Could not bind socket - address is protected!\n");
            break;
        case EADDRINUSE:
            printf("Could not bind socket - address already in use or "
                    "address was set to 0 and all ephemeral ports were already in use!\n");
            break;
        case EBADF:
            printf("Could not bind socket - %i is not a valid fd!\n", sockfd);
            break;
        case EINVAL:
            printf("Could not bind socket - socket is either already bound to an address or "
                    "addrlen is wrong or addr is not a valid address for this socket's domain!\n");
            break;
        case ENOTSOCK:
            printf("Could not bind socket - fd %i does not refer to a socket!\n", sockfd);
            break;
        default:
            printf("Could not bind socket - operation failed with error code %i!\n", errno);
            break;
    }

    return -1;
}

static int handle_listen_err()
{
    switch (errno) {
        case EADDRINUSE:
            printf("Listen syscall failed - address already in use or all ephemeral ports taken!\n");
            break;
        case EBADF:
            printf("Listen syscall failed - sockfd is not a valid file descriptor. "
                    "As a user there is nothing you can do, this is a bug!\n");
            break;
        case ENOTSOCK:
            printf("Listen syscall failed - sockfd does not refer to a socket. "
                    "As a user there is nothing you can do, this is a bug!\n");
            break;
        case EOPNOTSUPP:
            printf("Listen syscall failed - socket is not of a type that supports the listen() operation. "
                    "As a user there is nothing you can do, this is a bug!\n");
            break;
        default:
            printf("Listen syscall failed with error code %i!\n", errno);
            break;
    }

    return -1;
}

static int handle_close_err(const int sockfd)
{
    switch(errno) {
        case EBADF:
            printf("Close syscall failed - %i isn't a valid open fd!\n", sockfd);
            break;
        case EINTR:
            printf("Close syscall was interrupted by a signal!\n");
            if (close(sockfd) != 0) {
                /* TODO (GM): Can this cause an infinite loop? */
                handle_close_err(sockfd);
            }

            break;
        case EIO:
            printf("Close syscall failed - I/O error!\n");
            break;
        default:
            printf("Close syscall failed with error code %i!\n", errno);
            break;
    }

    return -1;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "syscall.h"
#include "socket.h"

#define IP "127.0.0.1"
#define SERVER_PORT 8080
#define BACKLOG_SIZE 100

#define MAX_MESSAGE_SIZE 1024

static int handle_socket_err();
static int handle_bind_err(const int sockfd);
static int handle_listen_err();
static int handle_accept_err();
static int handle_recv_err();
static int handle_close_err(const int sockfd);

int main(void)
{
    /* AF_INET sockets can either be connection-oriented SOCK_STREAM
     * or connectionless SOCK_DGRAM, but not SOCK_SEQPACKET!
     */
    const int sockfd = syscall(SOCKET_SYSCALL_NO, AF_INET, SOCK_STREAM, 0);
    int connected_sockfd;

    char msg[MAX_MESSAGE_SIZE];
    struct sockaddr addr;

    if (sockfd == -1) {
        return handle_socket_err();
    }

    if (construct_sockaddr(&addr, sizeof(addr), IP, SERVER_PORT) != 0) {
        printf("Could not construct server sockaddr %s:%i, error code %i!\n", IP, SERVER_PORT, errno);
        return -1;
    }

    if (syscall(BIND_SYSCALL_NO, sockfd, &addr, sizeof(addr)) != 0) {
        return handle_bind_err(sockfd);
    }

    if (syscall(LISTEN_SYSCALL_NO, sockfd, BACKLOG_SIZE) != 0) {
        return handle_listen_err();
    }

    printf("Waiting for connections on %s:%i...\n", IP, SERVER_PORT);

    if ((connected_sockfd = syscall(ACCEPT_SYSCALL_NO, sockfd, NULL, NULL)) == -1) {
        return handle_accept_err();
    }

    /* TODO (GM): Handle messages larger than MAX_MESSAGE_SIZE -> Set rcvbuf size somehow */
    /* TODO (GM): Set the MSG_DONTWAIT Flag to prevent blocking? */
    while (syscall(RECVFROM_SYSCALL_NO, connected_sockfd, msg, MAX_MESSAGE_SIZE, 0, NULL, NULL) != 0) {
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
    if (syscall(CLOSE_SYSCALL_NO, connected_sockfd) != 0) {
        handle_close_err(connected_sockfd);
    }

    printf("Closing socket %i...\n", sockfd);
    if (syscall(CLOSE_SYSCALL_NO, sockfd) != 0) {
        handle_close_err(sockfd);
    }

    printf("Done!\n");
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

static int handle_accept_err()
{
    switch (errno) {
        /* The following two are the same - at least in C */
        /* case EWOULDBLOCK: */
        case EAGAIN:
            printf("Could not accept - socket is marked nonblocking and no connections are present to be accepted.\n");
            break;
        case EBADF:
            printf("Could not accept - sockfd is not an open fd.\n");
            break;
        case ECONNABORTED:
            printf("Could not accept - connection has been aborted.\n");
            break;
        case EFAULT:
            printf("Could not accept - addr argument is not in a writeable part of the user address space.\n");
            break;
        case EINTR:
            printf("Could not accept - syscall was interrupted by a signal before a connection arrived.\n");
            break;
        case EINVAL:
            printf("Could not accept - socket is not listening for connections or addrlen is invalid or invalid value in flags.\n");
            break;
        case EMFILE:
            printf("Could not accept - The per-process limit on the number of open fds has been reached.\n");
            break;
        case ENFILE:
            printf("Could not accept - The system-wide limit on the total number of open files has been reached.\n");
            break;
        case ENOBUFS:
        case ENOMEM:
            printf("Could not accept - not enough free memory. This often indicates that the memory allocation is limited by "
                    "the socket buffer limits rather than the system memory.\n");
            break;
        case ENOTSOCK:
            printf("Could not accept - sockfd does not refer to a socket.\n");
            break;
        case EOPNOTSUPP:
            printf("Could not accept - the referenced socket is not of type SOCK_STREAM.\n");
            break;
        case EPROTO:
            printf("Could not accept - protocol error.\n");
            break;
        case EPERM:
            printf("Could not accept - firewall rules forbid connection.\n");
            break;
        default:
            printf("Could not accept - error code %i.\n", errno);
            break;
    }

    return -1;
}

static int handle_recv_err()
{
    switch (errno) {
        /* The following two are the same - at least in C */
        /* case EWOULDBLOCK: */
        case EAGAIN:
            printf("Could not recv - socket is marked nonblocking and recv would block or recv timeout expired before data has arrived.\n");
            break;
        case EBADF:
            printf("Could not recv - sockfd is an invalid fd.\n");
            break;
        case ECONNREFUSED:
            printf("Could not recv - remote host refused the network connection (typically because it is not running the requested service).\n");
            break;
        case EFAULT:
            printf("Could not recv - recv buffer pointer points to outside the process's address space.\n");
            break;
        case EINTR:
            printf("Could not recv - recv was sigkilled.\n");
            break;
        case EINVAL:
            printf("Could not recv - invalid argument.\n");
            break;
        case ENOMEM:
            printf("Could not recv - could not allocate sufficient memory.\n");
            break;
        case ENOTCONN:
            printf("Could not recv - socket is associated with a connection-oriented protocol and has not been connected.\n");
            break;
        case ENOTSOCK:
            printf("Could not recv - sockfd does not refer to a socket.\n");
            break;
        default:
            printf("Could not recv - error code %i.\n", errno);
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
            if (syscall(CLOSE_SYSCALL_NO, sockfd) != 0) {
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

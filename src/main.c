#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* TODO (GM): Technically stdint is new with C99... */
#include <stdint.h>

/* TODO (GM): The following are not C std libraries - can they be replaced? */
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
#define PORT 8080
#define BACKLOG_SIZE 100

#define MAX_MESSAGE_SIZE 1024

static int get_ipv4_bytes(char *buf, char *ipv4);
static int construct_sa_family(char *buf, char *ipv4, const uint16_t port);

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
    if (construct_sa_family(addr.sa_data, IP, PORT) != 0) {
        printf("Could not convert %s:%i to address, check your configuration!\n", IP, PORT);
        return -1;
    }

    if (bind(sockfd, &addr, sizeof(addr)) != 0) {
        return handle_bind_err(sockfd);
    }

    if (listen(sockfd, BACKLOG_SIZE) != 0) {
        return handle_listen_err();
    }

    /* TODO (GM): Handle messages larger than MAX_MESSAGE_SIZE -> Set rcvbuf size somehow */
    /* TODO (GM): Set the MSG_DONTWAIT Flag to prevent blocking? */
    while (recv(sockfd, msg, MAX_MESSAGE_SIZE, 0) != 0) {
        /* TODO (GM):
         * - Handle all possible error codes
         * - Build state machine that spins(?) if not connected
         */
        if (errno == ENOTCONN) {
            printf("Socket not connected!\n");
            sleep(1);
            continue;
        }

        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            printf("Error during recv: %i\n", errno);
        }
    }

    printf("Received a message: %s\n", msg);

    if (close(sockfd) != 0) {
        return handle_close_err(sockfd);
    }

    return 0;
}

static int get_ipv4_bytes(char *buf, char *ipv4)
{
    int i = 0;
    char tmp_buf[4];
    memset(tmp_buf, '\0', sizeof(tmp_buf));

    while (*ipv4) {
        if (*ipv4 != '.') {
            if (i >= 3) {
                /* ipv4 is not a valid ipv4! */
                return -1;
            }

            tmp_buf[i++] = *ipv4++;
            continue;
        }

        *buf++ = (uint8_t)atoi(tmp_buf);
        memset(tmp_buf, '\0', sizeof(tmp_buf));

        ipv4++;
        i = 0;
    }

    *buf++ = (uint8_t)atoi(tmp_buf);
    memset(tmp_buf, '\0', sizeof(tmp_buf));

    return 0;
}

static int construct_sa_family(char *buf, char *ipv4, const uint16_t port)
{
    char ipv4_bytes[5];
    ipv4_bytes[4] = 0;

    printf("String ip: %s\n", ipv4);
    if (get_ipv4_bytes(ipv4_bytes, ipv4) != 0) {
        return -1;
    }

    printf("Conv ip: %i.%i.%i.%i\n", ipv4_bytes[0], ipv4_bytes[1], ipv4_bytes[2], ipv4_bytes[3]);

    memcpy(buf, &port, 2);
    memcpy(buf + 2, ipv4_bytes, 4);
    buf[7] = '\0';

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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/* TODO (GM): Technically stdint is new with C99... */
#include <stdint.h>

/* TODO (GM): The following are not C std libraries - can they be replaced? */
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
#define SERVER_PORT 8080

#define MAX_MESSAGE_SIZE 1024

static int construct_sockaddr(struct sockaddr *addr, const unsigned addrlen, const char *ipv4, const uint16_t port);
static int get_ipv4_bytes(char *buf, const char *ipv4);
static int construct_sa_data(char *buf, const char *ipv4, const uint16_t port);
static int handle_connect_error();
static int handle_send_error();

int main(void)
{
    const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int bytes_sent;

    /* char msg[MAX_MESSAGE_SIZE]; */
    struct sockaddr addr;

    if (sockfd == -1) {
        printf("Error %i during socket creation!\n", errno);
        return -1;
    }

    if (construct_sockaddr(&addr, sizeof(addr), IP, SERVER_PORT) != 0) {
        printf("Could not construct server sockaddr %s:%i, error code %i!\n", IP, SERVER_PORT, errno);
        return -1;
    }

    printf("Trying to connect to %s:%i...\n", IP, SERVER_PORT);

    while(1) {
        if (connect(sockfd, &addr, sizeof(addr)) == 0) {
            printf("Connection established.\n");
            break;
        }

        handle_connect_error();
        sleep(1);
    }

    printf("Sending test request...\n");
    if ((bytes_sent = send(sockfd, "Test", 5, 0)) == -1) {
        return handle_send_error();
    }

    printf("Successfully sent %i bytes!\n", bytes_sent);

    /* TODO (GM): Error handling -> Always close sockets during failure! */
    printf("Closing socket %i...\n", sockfd);
    if (close(sockfd) != 0) {
        printf("Close syscall failed with error code %i!\n", errno);
    }

    printf("Done!\n");

    return 0;
}

static int construct_sockaddr(struct sockaddr *addr, const unsigned addrlen, const char *ipv4, const uint16_t port)
{
    memset(addr, 0, addrlen);
    addr->sa_family = AF_INET;
    if (construct_sa_data(addr->sa_data, ipv4, port) != 0) {
        printf("Could not convert %s:%i to address, check your configuration!\n", ipv4, port);
        return -1;
    }

    return 0;
}

static int get_ipv4_bytes(char *buf, const char *ipv4)
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

static int construct_sa_data(char *buf, const char *ipv4, const uint16_t port)
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

static int handle_connect_error()
{
    switch (errno) {
        case EACCES:
        case EPERM:
            printf("Could not connect to socket - permission error!\n");
            break;
        case EADDRINUSE:
            printf("Could not connect to socket - local address already in use!\n");
            break;
        case EADDRNOTAVAIL:
            printf("Could not connect to socket - socket has not been bound to address and "
                    "when trying to bind it to an ephemeral port all ephemeral ports were already in use!\n");
            break;
        case EAFNOSUPPORT:
            printf("Could not connect to socket - the passed address did not have the correct address in its sa_family field!\n");
            break;
        case EAGAIN:
            printf("Could not connect to socket - Unsufficient entries in the routing cache!\n");
            break;
        case EALREADY:
            printf("Could not connect to socket - socket is nonblocking and previous connection attempt has not been completed!\n");
            break;
        case EBADF:
            printf("Could not connect to socket - sockfd is not a valid open fd!\n");
            break;
        case ECONNREFUSED:
            printf("Could not connect to socket - Connection refused, no one is listening on the remote address!\n");
            break;
        case EFAULT:
            printf("Could not connect to socket - socket addr is outside the user's address space!\n");
            break;
        case EINPROGRESS:
            printf("Could not connect to socket - Socket is nonblocking and connection cannot be completed immediately.\n");
            break;
        case EINTR:
            printf("Could not connect to socket - syscall was interrupted by a caught signal!\n");
            break;
        case EISCONN:
            printf("Could not connect to socket - socket is already connected.\n");
            return 0;
        case ENETUNREACH:
            printf("Could not connect to socket - network unreachable.\n");
            break;
        case ENOTSOCK:
            printf("Could not connect to socket - sockfd does not refer to a socket!\n");
            break;
        case EPROTOTYPE:
            printf("Could not connect to socket - socket type does not support the requested protocol.\n");
            break;
        case ETIMEDOUT:
            printf("Could not connect to socket - timeout.\n");
            break;
        default:
            printf("Could not connect - error code %i.\n", errno);
            break;
    }

    return -1;
}

static int handle_send_error()
{
    switch (errno) {
        case EACCES:
            printf("Could not send - EACCES error!\n");
            break;
        /* The following two are the same - in C at least */
        case EAGAIN:
        /* case EWOULDBLOCK: */
            printf("Could not send - socket has not been bound to an address and no ephemeral ports were available OR "
                    "tried to send a blocking request to a nonblocking socket.\n");
            break;
        case EALREADY:
            printf("Could not send - another fast open is in progress!\n");
            break;
        case EBADF:
            printf("Could not send - sockfd is not a valid open fd!\n");
            break;
        case ECONNRESET:
            printf("Could not send - connection reset by peer!\n");
            break;
        case EDESTADDRREQ:
            printf("Could not send - the socket is not in connection mode and no peer address is set!\n");
            break;
        case EFAULT:
            printf("Could not send - and invalid user space adress was specfied for an argument!\n");
            break;
        case EINTR:
            printf("Could not send - a signal occurred before any data was transmitted!\n");
            break;
        case EINVAL:
            printf("Could not send - invalid argument passed!\n");
            break;
        case EISCONN:
            printf("Could not send - the connection-mode socket was connected already but a recipient was specified.\n");
            break;
        case EMSGSIZE:
            printf("Could not send - the socket type requires that messages be sent atomically, and the size of the message "
                    "to be sent made this impossible.\n");
            break;
        case ENOBUFS:
            printf("Could not send - the output interface for a network interface was full.\n");
            break;
        case ENOMEM:
            printf("Could not send - no memory available.\n");
            break;
        case ENOTCONN:
            printf("Could not send - the socket is not connected, and no target has been given.\n");
            break;
        case ENOTSOCK:
            printf("Could not send - sockfd does not refer to a socket!\n");
            break;
        case EOPNOTSUPP:
            printf("Could not send - some bit in the flags argument is inappropriate for this socket type!\n");
            break;
        case EPIPE:
            printf("Could not send - the local end has been shut down on a connection oriented socket!\n");
            break;
        default:
            printf("Could not send - error code %i.\n", errno);
            break;
    }

    return -1;
}

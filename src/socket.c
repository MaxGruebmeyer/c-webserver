#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "socket.h"
#include "syscall.h"
#include "structs.h"
#include "errorhandler.h"

static int construct_sockaddr(struct sockaddr *addr, const unsigned addrlen, const char *ipv4, const unsigned short port);
static int get_ipv4_bytes(char *buf, const char *ipv4);
static void get_port_bytes(char *buf, const unsigned short port);

int create_listening_socket(int *sockfd, const char *ip, const unsigned short port, const int backlog_size)
{
    struct sockaddr addr;

    /* AF_INET sockets can either be connection-oriented SOCK_STREAM
     * or connectionless SOCK_DGRAM, but not SOCK_SEQPACKET!
     */
    if ((*sockfd = socket(AF_INET, SOCK_STREAM)) == -1) {
        return handle_socket_err();
    }

    if (construct_sockaddr(&addr, sizeof(addr), ip, port) != 0) {
        printf("Could not construct server sockaddr %s:%i, error code %i!\n", ip, port, errno);
        return -1;
    }

    printf("Trying to start server on %u.%u.%u.%u:%i with socket id %i\n",
            addr.sa_data.addr.ip[0], addr.sa_data.addr.ip[1], addr.sa_data.addr.ip[1], addr.sa_data.addr.ip[2],
            ((unsigned char)addr.sa_data.addr.port[0] << 8) + (unsigned char)addr.sa_data.addr.port[1],
            *sockfd);

    if (bind(*sockfd, &addr, sizeof(addr)) != 0) {
        close_socket(sockfd);
        return handle_bind_err(*sockfd);
    }

    if (listen(*sockfd, backlog_size) != 0) {
        close_socket(sockfd);
        return handle_listen_err();
    }

    return 0;
}

void close_socket(int *socket_fd)
{
    if (*socket_fd < 0) {
        printf("Could not close socket, negative value %i implies socket already closed!\n", *socket_fd);
        return;
    }

    printf("Closing socket %i...\n", *socket_fd);
    if (close(*socket_fd) != 0) {
        printf("Close failed, handling error!\n");
        handle_close_err(*socket_fd);
    } else {
        printf("Socket closed successfully!\n");
    }

    *socket_fd = -1;
}

/* TODO (GM): Unit test this method! */
static int construct_sockaddr(struct sockaddr *addr, const unsigned addrlen, const char *ipv4, const unsigned short port)
{
    memset(addr, 0, addrlen);
    addr->sa_family = AF_INET;

    get_port_bytes(addr->sa_data.addr.port, port);
    if (get_ipv4_bytes(addr->sa_data.addr.ip, ipv4) != 0) {
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

        *buf++ = (unsigned char)atoi(tmp_buf);
        memset(tmp_buf, '\0', sizeof(tmp_buf));

        ipv4++;
        i = 0;
    }

    *buf++ = (unsigned char)atoi(tmp_buf);
    memset(tmp_buf, '\0', sizeof(tmp_buf));

    return 0;
}

static void get_port_bytes(char *buf, const unsigned short port)
{
    union {
        unsigned short port;
        struct {
            /* TODO (GM): Why does the smaller part come before the larger part? */
            /* TODO (GM): Does this have something to do with little Endian? */
            unsigned char smaller, larger;
        } bytes;
    } port_union;

    port_union.port = port;

    *buf++ = port_union.bytes.larger;
    *buf++ = port_union.bytes.smaller;
}

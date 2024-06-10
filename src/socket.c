#include <string.h>
#include <errno.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "socket.h"
#include "errorhandler.h"

#include "logging.h"

static int construct_sockaddr(struct sockaddr_in *addr, const unsigned addrlen, const char *ipv4, const unsigned short port);

int create_listening_socket(int *sockfd, const char *ip, const unsigned short port, const int backlog_size)
{
    struct sockaddr_in addr;

    /* AF_INET sockets can either be connection-oriented SOCK_STREAM
     * or connectionless SOCK_DGRAM, but not SOCK_SEQPACKET!
     */
    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return handle_socket_err();
    }

    if (construct_sockaddr(&addr, sizeof(addr), ip, port) != 0) {
        log_fatal("Could not construct server sockaddr %s:%i, error code %i!\n", ip, port, errno);
        return -1;
    }

    // TODO (GM): Beautify this!
    union addr {
        char str[4];
        struct in_addr ip;
    } addr_union;
    addr_union.ip = addr.sin_addr;

    log_info("Trying to start server on %u.%u.%u.%u:%i with socket id %i\n",
            addr_union.str[0], addr_union.str[1], addr_union.str[2], addr_union.str[3],
            ntohs(addr.sin_port), *sockfd);

    if (bind(*sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
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
        log_error("Could not close socket, negative value %i implies socket already closed!\n", *socket_fd);
        return;
    }

    log_debug("Closing socket %i...\n", *socket_fd);
    if (close(*socket_fd) != 0) {
        log_error("Close failed, handling error!\n");
        handle_close_err(*socket_fd);
    } else {
        log_debug("Socket closed successfully!\n");
    }

    *socket_fd = -1;
}

/* TODO (GM): Unit test this method! */
static int construct_sockaddr(struct sockaddr_in *addr, const unsigned addrlen, const char *ipv4, const unsigned short port)
{
    memset(addr, 0, addrlen);
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    inet_pton(AF_INET, ipv4, &addr->sin_addr);

    return 0;
}

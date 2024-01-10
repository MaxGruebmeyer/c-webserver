#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "errorhandler.h"
#include "syscall.h"
#include "socket.h"

#define IP "127.0.0.1"
#define PORT 8080
#define BACKLOG_SIZE 100

#define MAX_REQ_SIZE 1024
#define MAX_RES_SIZE 2 * 1024

static void interrupt_sighandler(int _);
static int send_dummy_response(const int sockfd);
static void close_socket(int *sockfd);

static int sockfd = -1;
static int connected_sockfd = -1;

int main(void)
{
    char msg[MAX_REQ_SIZE];
    struct sockaddr addr;

    /* Simple interrupt handler to properly shut down sockets on <Ctrl-C> */
    signal(SIGINT, interrupt_sighandler);

    /* AF_INET sockets can either be connection-oriented SOCK_STREAM
     * or connectionless SOCK_DGRAM, but not SOCK_SEQPACKET!
     */
    if ((sockfd = socket(AF_INET, SOCK_STREAM)) == -1) {
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
        close_socket(&sockfd);
        return handle_bind_err(sockfd);
    }

    if (listen(sockfd, BACKLOG_SIZE) != 0) {
        close_socket(&sockfd);
        return handle_listen_err();
    }

    printf("Watiting for connections...\n");

    if ((connected_sockfd = accept(sockfd)) == -1) {
        close_socket(&sockfd);
        return handle_accept_err();
    }

    /* TODO (GM): Fork process here? */

    /* TODO (GM): Handle messages larger than MAX_REQ_SIZE -> Set rcvbuf size somehow */
    /* TODO (GM): Set the MSG_DONTWAIT Flag to prevent blocking? */
    while (recvfrom(connected_sockfd, msg, MAX_REQ_SIZE) != 0) {
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

        printf("Got an EAGAIN or EWOULDBLOCK, breaking...\n");
        break;
    }

    printf("Received a message: \"%s\"\n", msg);
    if (send_dummy_response(connected_sockfd) == -1) {
        printf("Sending dummy response failed!\n");
        close_socket(&connected_sockfd);
        close_socket(&sockfd);
    }

    printf("Successfully sent dummy response!\n");

    close_socket(&connected_sockfd);
    close_socket(&sockfd);

    printf("Done!\n");
    return 0;
}

# pragma GCC diagnostic ignored "-Wunused-parameter"

static void interrupt_sighandler(int _)
{
    printf("\033[33mInterrupt signal received, closing sockets!\033[0m\n");

    close_socket(&connected_sockfd);
    close_socket(&sockfd);

    printf("\033[33mAll sockets closed, exiting.\033[0m\n");
    exit(0);
}

# pragma GCC diagnostic error "-Wunused-parameter"

static int send_dummy_response(const int sockfd)
{
    int i = 0;
    char res[MAX_RES_SIZE];
    char *body;

    for (; i < MAX_RES_SIZE; i++) {
        res[i] = '\0';
    }

    body = "<h1>Hallo Minesweeper-Enthusiasten</h1>\n"
        "<b>Ich hab nen Webserver, was habt ihr?</b>";
    if(!sprintf(res, "HTTP/1.1 200 OK\r\n"
        "Content-Length: %lu\r\n"
        "Content-Type: text/html\r\n\r\n"
        "%s\r\n",
        strlen(body), body)) {
        printf("Could not assign to body!\n");
        return -1;
    }

    printf("Returning dummy response with len %lu:\n%s\n", strlen(res), res);
    return sendto(sockfd, res, strlen(res));
}

static void close_socket(int *socket_fd)
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

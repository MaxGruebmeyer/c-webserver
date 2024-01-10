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

#define MAX_INCOMING_CONNECTIONS 1024

#define MAX_REQ_SIZE 1024
#define MAX_RES_SIZE 2 * 1024

#define NO_CHILD -1

static void interrupt_sighandler(int _);
static void interrupt_children(void);

static int start_accepting(void);
static int start_listening(void);
static int send_dummy_response(void);
static void close_socket(int *socket_fd);

static void reset_child_pids();

static int sockfd = -1;
static int child_pids[MAX_INCOMING_CONNECTIONS];

int main(void)
{
    struct sockaddr addr;

    /* Simple interrupt handler to properly shut down sockets on <Ctrl-C> */
    signal(SIGINT, interrupt_sighandler);
    reset_child_pids();

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

    printf("Trying to start server on %u.%u.%u.%u:%i with socket id %i\n",
            addr.sa_data.addr.ip[0], addr.sa_data.addr.ip[1], addr.sa_data.addr.ip[1], addr.sa_data.addr.ip[2],
            ((unsigned char)addr.sa_data.addr.port[0] << 8) + (unsigned char)addr.sa_data.addr.port[1],
            sockfd);

    if (bind(sockfd, &addr, sizeof(addr)) != 0) {
        close_socket(&sockfd);
        return handle_bind_err(sockfd);
    }

    if (listen(sockfd, BACKLOG_SIZE) != 0) {
        close_socket(&sockfd);
        return handle_listen_err();
    }

    printf("Watiting for connections...\n");
    while (start_accepting() == 0);

    printf("\033[32mDone for this process, killing possible children and closing socket...\033[0m\n");

    interrupt_children();
    close_socket(&sockfd);

    printf("Done!\n");
    return 0;
}

# pragma GCC diagnostic ignored "-Wunused-parameter"

static void interrupt_sighandler(int _)
{
    /* TODO (GM): Set variable so new connections are not accepted! */

    printf("\033[33mInterrupt signal received, closing sockets!\033[0m\n");

    /* Send SIGINT to all children and wait until the exit so they close their respective sockets */
    interrupt_children();

    /* Then we only need to close our socket. */
    close_socket(&sockfd);

    printf("\033[33mAll sockets closed, exiting.\033[0m\n");
    exit(0);
}

# pragma GCC diagnostic error "-Wunused-parameter"

static void interrupt_children(void)
{
    int i = 0;

    /* Don't loop and more importantly don't print anything for child processes. */
    if (child_pids[0] == NO_CHILD) {
        return;
    }

    printf("Sending SIGINT to all child processes...\n");

    /* TODO (GM): What happens when a child connection is closed? How do we free the pid? */
    while (i < MAX_INCOMING_CONNECTIONS && child_pids[i] != NO_CHILD) {
        /* TODO (GM): Verify children handle SIGINT correctly! */

        printf("Sending SIGINT to child process %i.\n", child_pids[i]);
        kill(child_pids[i], SIGINT);

        /* TODO (GM): Wait for completion! */
    }

    printf("All children notified!\n");
    printf("Waiting for children to die...\n");

    i = 0;
    while (i < MAX_INCOMING_CONNECTIONS && child_pids[i] != NO_CHILD) {
        printf("Waiting for child process %i to complete...\n", child_pids[i]);
        wait4(child_pids[i]);
    }

    printf("All children killed!\n");
}

static int start_accepting(void)
{
    int child_sockfd = accept(sockfd);
    int fork_res;

    if (child_sockfd == -1) {
        close_socket(&sockfd);

        /* TODO (GM): Is this correct here? Or should we just abort everything like when we cannot fork? */
        return handle_accept_err();
    }

    fork_res = fork();
    if (fork_res == -1) {
        printf("\033[31mCould not fork process, aborting!\033[0m\n");

        close_socket(&child_sockfd);
        close_socket(&sockfd);

        return -1;
    }

    if (fork_res == 0) {
        /* Child process */
        sockfd = child_sockfd;

        /* Child process should not have any more children. */
        reset_child_pids();

        printf("\033[32mReceived a new connection in newly spawned child process on socket %i!\033[0m\n", sockfd);

        start_listening();

        /* Return non-zero status code to prevent infinite loops */
        return 1;
    } else {
        /* Parent process */
        int i = 0;
        while (i < MAX_INCOMING_CONNECTIONS && child_pids[i++] != NO_CHILD);

        if (i == MAX_INCOMING_CONNECTIONS) {
            printf("\033[33mMaximum number of concurrent connections reached!\033[0m\n");
            return -1;
        }

        child_pids[i] = fork_res;
    }

    return 0;
}

static int start_listening(void)
{
    char msg[MAX_REQ_SIZE];
    printf("Received an incoming connection, waiting for data...\n");

    /* TODO (GM): Handle messages larger than MAX_REQ_SIZE -> Set rcvbuf size somehow */
    /* TODO (GM): Set the MSG_DONTWAIT Flag to prevent blocking? */
    while (recvfrom(sockfd, msg, MAX_REQ_SIZE) != 0) {
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
    if (send_dummy_response() == -1) {
        printf("Sending dummy response failed!\n");
        return -1;
    }

    printf("Successfully sent dummy response!\n");
    return 0;
}

static int send_dummy_response(void)
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

static void reset_child_pids()
{
    int i = 0;
    for (; i < MAX_INCOMING_CONNECTIONS; i++) {
        child_pids[i] = NO_CHILD;
    }
}

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
static void exit_process(const int exit_code);

static int start_accepting(void);
static int start_listening(void);
static int send_dummy_response(void);

static void reset_child_pids();

static int sockfd = -1;
static int child_pids[MAX_INCOMING_CONNECTIONS];

int main(void)
{
    /* Simple interrupt handler to properly shut down sockets on <Ctrl-C> */
    signal(SIGINT, interrupt_sighandler);
    reset_child_pids();

    if (create_listening_socket(&sockfd, IP, PORT, BACKLOG_SIZE) != 0) {
        printf("Socket creation failed!\n");
        return -1;
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
    printf("\033[31mInterrupt signal received!\033[0m\n");

    exit_process(0);
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
    /* TODO (GM): Also sockfd just increments -> where is the limit? When is a fd freed again? What are the implication for our connection limit? */
    while (i < MAX_INCOMING_CONNECTIONS && child_pids[i] != NO_CHILD) {
        printf("Sending SIGINT to child process %i.\n", child_pids[i]);
        kill(child_pids[i], SIGINT);
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

static void exit_process(const int exit_code)
{
    printf("\033[33mClosing sockets...\033[0m\n");

    /* Send SIGINT to all children and wait until the exit so they close their respective sockets */
    interrupt_children();

    /* Then we only need to close our socket. */
    /* TODO (GM): Properly terminate the connection TCP style before just straight up closing the socket! */
    close_socket(&sockfd);

    printf("\033[33mAll sockets closed, exiting.\033[0m\n");
    exit(exit_code);
}

static int start_accepting(void)
{
    int child_sockfd = accept(sockfd);
    int fork_res;

    if (child_sockfd == -1) {
        close_socket(&sockfd);

        /* TODO (GM): Does this make sense? Or should we just abort everything like when we cannot fork? */
        return handle_accept_err();
    }

    /* Result of -1 indicates failure, 0 indicates we are in the child and >0 means we are in the parent. */
    fork_res = fork();

    /* Error case */
    if (fork_res == -1) {
        printf("\033[31mCould not fork process, aborting!\033[0m\n");

        close_socket(&child_sockfd);
        close_socket(&sockfd);

        return -1;
    }

    /* Parent process */
    if (fork_res > 0) {
        int i = 0;
        while (i < MAX_INCOMING_CONNECTIONS && child_pids[i++] != NO_CHILD);

        if (i == MAX_INCOMING_CONNECTIONS) {
            printf("\033[33mMaximum number of concurrent connections reached!\033[0m\n");
            return -1;
        }

        child_pids[i] = fork_res;
        return 0;
    }

    /* Child process */
    sockfd = child_sockfd;
    reset_child_pids();

    printf("\033[32mReceived a new connection in newly spawned child process on socket %i!\033[0m\n", sockfd);
    while (start_listening() == 0);

    /* exit_process kills the current process by calling exit(), there is absolutely no reason for a return statement here! */
#pragma GCC diagnostic ignored "-Wreturn-type"
    exit_process(0);
}
#pragma GCC diagnostic error "-Wreturn-type"

static int start_listening(void)
{
    char msg[MAX_REQ_SIZE];

    long bytes_received = -1;
    long bytes_sent = -1;

    printf("Waiting for data...\n");

    /* TODO (GM): Handle messages larger than MAX_REQ_SIZE -> Set rcvbuf size somehow */
    /* TODO (GM): Set the MSG_DONTWAIT Flag to prevent blocking? */
    while ((bytes_received = recvfrom(sockfd, msg, MAX_REQ_SIZE)) != 0) {
        /* TODO (GM): Handle all the possible error codes! */
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

    if (bytes_received == 0 || *msg == '\0') {
        printf("Received empty string, signaling termination of connection!\n");
        return -1;
    }

    printf("\033[35mReceived a message with length %li on socket %i:\033[0m\n--- REQUEST START ---\n%s\n--- REQUEST END ---\n", bytes_received, sockfd, msg);
    if ((bytes_sent = send_dummy_response()) < 0) {
        printf("Sending dummy response failed with return value %li!\n", bytes_sent);
        handle_send_error();

        return -1;
    }

    printf("Successfully sent dummy response with length %li!\n", bytes_sent);
    return 0;
}

static int send_dummy_response(void)
{
    int i = 0;
    char *body = "<h1>Hallo Minesweeper-Enthusiasten</h1>\n"
        "<b>Ich hab nen Webserver, was habt ihr?</b>\n";

    char res[MAX_RES_SIZE];
    for (; i < MAX_RES_SIZE; i++) {
        res[i] = '\0';
    }

    if(!sprintf(res, "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Encoding: UTF-8\r\n"
        "Content-Length: %lu\r\n"
        "\r\n"
        "%s"
        "\r\n\r\n",
        strlen(body), body)) {
        printf("Could not assign to body!\n");
        return -1;
    }

    printf("\033[36mSending response via socket %i with len %lu:\033[0m\n--- RESPONSE START ---\n%s\n--- RESPONSE END ---\n", sockfd, strlen(res), res);
    return sendto(sockfd, res, strlen(res));
}

static void reset_child_pids()
{
    int i = 0;
    for (; i < MAX_INCOMING_CONNECTIONS; i++) {
        child_pids[i] = NO_CHILD;
    }
}

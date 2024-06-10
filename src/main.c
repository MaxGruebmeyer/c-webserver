#define _POSIX_SOURCE

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include "errorhandler.h"
#include "reqhandler.h"
#include "logging.h"
#include "socket.h"

#define IP "0.0.0.0"
#define PORT 8080
#define BACKLOG_SIZE 100

#define MAX_INCOMING_CONNECTIONS 1024

#define MAX_REQ_SIZE 1024
#define MAX_RES_SIZE 2 * 1024

#define CHILD_NEVER_INITIALIZED_PID -1
#define CHILD_RESET_PID 0

struct Child {
    int pid;
    int sockfd;
};

static void sigint_handler();
static void sigchld_handler();
static void interrupt_children(void);
static void kill_everything();

static int start_accepting(void);
static int start_listening(void);

static void reset_children();

static int sockfd = -1;
static struct Child children[MAX_INCOMING_CONNECTIONS];

// TODO (GM): Clean up main!
int main(void)
{
    /* TODO (GM): Can we switch to sigaction without too much hassle? */
    /* Simple interrupt handler to properly shut down sockets on <Ctrl-C> */
    signal(SIGINT, sigint_handler);

    /* Handler that releases child resources once the child dies. */
    signal(SIGCHLD, sigchld_handler);

    reset_children();
    log_debug("Initializing routes!\n");

    // TODO (GM): Support html loading and add actual routes!
    route_init();
    add_route("/Linux", "<h1>Without Linux this would not be possible!</h1>");
    add_route("/Matheraum", "<h1>Komm in den offenen Matheraum!</h1>");

    log_debug("Initialized all routes!\n");

    if (create_listening_socket(&sockfd, IP, PORT, BACKLOG_SIZE) != 0) {
        log_error("Socket creation failed!\n");
        return -1;
    }

    log_debug("Watiting for connections...\n");
    while (start_accepting() == 0);

    log_info("\033[32mDone for this process, killing possible children and closing socket...\033[0m\n");

    kill_everything();

    log_info("Done!\n");
    return 0;
}

static void sigint_handler()
{
    /* Re-register signal */
    signal(SIGINT, sigint_handler);

    /* TODO (GM): Set variable so new connections are not accepted! */
    log_warn("\033[31mInterrupt signal received!\033[0m\n");

    kill_everything();
    exit(0);
}

/*
 * TODO (GM): Make this method safer (e.g. extract into its own process)
 *  because this method will get interrupted by any other signal!
 */
static void sigchld_handler()
{
    /* pid -1 to wait for any child. */
    /* options 1 (WNOHANG) for non-blocking wait */
    int child_pid = waitpid(-1, NULL, 1);
    int i;

    /* Re-register signal */
    signal(SIGCHLD, sigchld_handler);

    log_debug("Trying to remove child with pid %i from children array...\n", child_pid);

    for (i = 0; i < MAX_INCOMING_CONNECTIONS && children[i].pid != CHILD_NEVER_INITIALIZED_PID; i++) {
        if (children[i].pid != child_pid) {
            continue;
        }

        /* The child should have already closed its reference to the socket
         *  but we need to close our reference as well,
         *  otherwise the fd will NOT be reused! */
        close_socket(&children[i].sockfd);

        children[i].pid = CHILD_RESET_PID;
        children[i].sockfd = -1;

        log_debug("Child with pid %i removed successfully from children array!\n", child_pid);
        return;
    }

    log_fatal("Could not remove child with pid %i from children array!\n", child_pid);
}

static void interrupt_children(void)
{
    int i;

    /* Don't loop and more importantly don't print anything for child processes. */
    if (children[0].pid == CHILD_NEVER_INITIALIZED_PID) {
        return;
    }

    log_warn("Sending SIGINT to all child processes...\n");

    /* TODO (GM): What happens when a child connection is closed? How do we free the pid? */
    /* TODO (GM): Also sockfd just increments -> where is the limit? When is a fd freed again? What are the implication for our connection limit? */
    for (i = 0; i < MAX_INCOMING_CONNECTIONS && children[i].pid != CHILD_NEVER_INITIALIZED_PID; ++i) {
        if (children[i].pid == CHILD_RESET_PID) {
            continue;
        }

        log_warn("Sending SIGINT to child process %i.\n", children[i].pid);
        kill(children[i].pid, SIGINT);
    }

    log_warn("All children notified!\n");
    log_warn("Waiting for children to die...\n");

    /* TODO (GM): Is this loop still neccessary with the SIGCHLD sighandler? */
    for (i = 0; i < MAX_INCOMING_CONNECTIONS && children[i].pid != CHILD_NEVER_INITIALIZED_PID; ++i) {
        if (children[i].pid == CHILD_RESET_PID) {
            continue;
        }

        log_warn("Waiting for child process %i to complete...\n", children[i].pid);
        waitpid(children[i].pid, NULL, 0);
    }

    log_warn("All children killed!\n");
}

static void kill_everything()
{
    log_debug("\033[33mClosing sockets...\033[0m\n");

    /* Send SIGINT to all children and wait until the exit so they close their respective sockets */
    interrupt_children();

    /* Then we only need to close our socket. */
    /* TODO (GM): Properly terminate the connection TCP style before just straight up closing the socket! */
    close_socket(&sockfd);

    /* And don't forget to free the memory used by the routes */
    route_cleanup();

    log_debug("\033[33mAll sockets closed, exiting.\033[0m\n");
}

static int start_accepting(void)
{
    int child_sockfd = accept(sockfd, NULL, NULL);
    int fork_res;

    if (child_sockfd < 0) {
        /* TODO (GM): Fix */
        log_warn("Got a negative sockfd, probably because a child has been killed!\n");
        return 0;
    }

    /* Result of -1 indicates failure, 0 indicates we are in the child and >0 means we are in the parent. */
    fork_res = fork();

    /* Error case */
    if (fork_res == -1) {
        log_error("\033[31mCould not fork process, aborting!\033[0m\n");
        return -1;
    }

    /* Parent process */
    if (fork_res > 0) {
        int i = 0;
        for (; i < MAX_INCOMING_CONNECTIONS; ++i) {
            if (children[i].pid == CHILD_RESET_PID) {
                break;
            }

            if (children[i].pid == CHILD_NEVER_INITIALIZED_PID) {
                break;
            }
        }

        if (i == MAX_INCOMING_CONNECTIONS) {
            log_error("\033[33mMaximum number of concurrent connections reached!\033[0m\n");
            return -1;
        }

        children[i].pid = fork_res;
        children[i].sockfd = child_sockfd;

        return 0;
    }

    /* Child process */
    sockfd = child_sockfd;
    reset_children();

    log_debug("\033[32mReceived a new connection in newly spawned child process on socket %i!\033[0m\n", sockfd);
    while (start_listening() == 0);

    log_debug("\033[32mConnection served, killing child process...\033[0m\n");
    kill_everything();

    log_debug("\033[32mAll resources released, exiting.\033[0m\n");
    exit(0);
}

static int start_listening(void)
{
    char req[MAX_REQ_SIZE];
    char res[MAX_RES_SIZE];

    long bytes_received = -1;
    long bytes_sent = -1;

    if (sockfd < 0) {
        log_error("Sockfd %i was negative value!\n", sockfd);
        return 1;
    }

    log_debug("Waiting for data...\n");

    /* TODO (GM): Handle messages larger than MAX_REQ_SIZE -> Set rcvbuf size somehow */
    /* TODO (GM): Set the MSG_DONTWAIT Flag to prevent blocking? */
    while ((bytes_received = recvfrom(sockfd, req, MAX_REQ_SIZE, 0, NULL, NULL)) != 0) {
        /* TODO (GM): Handle all the possible error codes! */
        if (errno == ENOTCONN) {
            log_warn("Socket not connected!\n");
            continue;
        }

        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            handle_recv_err();
        }

        log_info("Got an EAGAIN or EWOULDBLOCK, breaking...\n");
        break;
    }

    if (bytes_received == 0 || *req == '\0') {
        log_debug("Received empty string, signaling termination of connection!\n");
        return -1;
    }

    log_debug("\033[35mReceived a message on socket %i.\033[0m\n", sockfd);
    log_trace("--- REQUEST START ---\n%s\n--- REQUEST END ---\n", req);

    if (handle_request(req, res, MAX_RES_SIZE) != 0) {
        log_fatal("COULD NOT HANDLE REQUEST, EPIC FAIL!\n");
        return -1;
    }

    log_debug("\033[36mSending response via socket %i.\033[0m\n", sockfd);
    log_trace("--- RESPONSE START ---\n%s\n--- RESPONSE END ---\n", res);

    if ((bytes_sent = sendto(sockfd, res, strlen(res), 0, NULL, 0)) < 0) {
        log_warn("Sending response failed with return value %li!\n", bytes_sent);
        handle_send_error();

        return -1;
    }

    log_debug("Successfully sent response with length %li!\n", bytes_sent);
    return 0;
}

static void reset_children()
{
    int i = 0;
    for (; i < MAX_INCOMING_CONNECTIONS; i++) {
        children[i].pid = CHILD_NEVER_INITIALIZED_PID;
    }
}

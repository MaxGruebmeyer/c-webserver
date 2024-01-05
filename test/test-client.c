#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "../src/syscall.h"
#include "../src/socket.h"
#include "../src/errorhandler.h"

#define IP "127.0.0.1"
#define SERVER_PORT 8080

#define MAX_MESSAGE_SIZE 1024

int main(void)
{
    const int sockfd = syscall(SOCKET_SYSCALL_NO, AF_INET, SOCK_STREAM, 0);
    int bytes_sent;

    /* char msg[MAX_MESSAGE_SIZE]; */
    struct sockaddr addr;

    if (sockfd == -1) {
        handle_socket_err();
        return -1;
    }

    if (construct_sockaddr(&addr, sizeof(addr), IP, SERVER_PORT) != 0) {
        printf("Could not construct server sockaddr %s:%i, error code %i!\n", IP, SERVER_PORT, errno);
        return -1;
    }

    printf("Trying to connect to %s:%i...\n", IP, SERVER_PORT);

    while(1) {
        if (syscall(CONNECT_SYSCALL_NO, sockfd, &addr, sizeof(addr)) == 0) {
            printf("Connection established.\n");
            break;
        }

        handle_connect_error();
        sleep(1);
    }

    printf("Sending test request...\n");
    if ((bytes_sent = syscall(SENDTO_SYSCALL_NO, sockfd, "Test", 5, 0)) == -1) {
        return handle_send_error();
    }

    printf("Successfully sent %i bytes!\n", bytes_sent);

    /* TODO (GM): Error handling -> Always close sockets during failure! */
    printf("Closing socket %i...\n", sockfd);
    if (syscall(CLOSE_SYSCALL_NO, sockfd) != 0) {
        printf("Close syscall failed with error code %i!\n", errno);
    }

    printf("Done!\n");

    return 0;
}

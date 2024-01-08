#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "../src/syscall.h"
#include "../src/socket.h"
#include "../src/errorhandler.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

#define MAX_MESSAGE_SIZE 1024

int main(void)
{
    const int sockfd = socket(AF_INET, SOCK_STREAM);
    int bytes_sent;

    /* char msg[MAX_MESSAGE_SIZE]; */
    struct sockaddr addr;

    if (sockfd == -1) {
        handle_socket_err();
        return -1;
    }

    if (construct_sockaddr(&addr, sizeof(addr), SERVER_IP, SERVER_PORT) != 0) {
        printf("Could not construct server sockaddr %s:%i, error code %i!\n", SERVER_IP, SERVER_PORT, errno);
        return -1;
    }

    printf("Trying to connect to %s:%i...\n", SERVER_IP, SERVER_PORT);

    while(1) {
        if (connect(sockfd, &addr, sizeof(addr)) == 0) {
            printf("Connection established.\n");
            break;
        }

        handle_connect_error();
        sleep(1);
    }

    printf("Sending test request...\n");
    if ((bytes_sent = sendto(sockfd, "Test", 5)) == -1) {
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

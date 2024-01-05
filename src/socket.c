#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "socket.h"

static int get_ipv4_bytes(char *buf, const char *ipv4);
static int construct_sa_data(char *buf, const char *ipv4, const unsigned short port);

int construct_sockaddr(struct sockaddr *addr, const unsigned addrlen, const char *ipv4, const unsigned short port)
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

        *buf++ = (unsigned char)atoi(tmp_buf);
        memset(tmp_buf, '\0', sizeof(tmp_buf));

        ipv4++;
        i = 0;
    }

    *buf++ = (unsigned char)atoi(tmp_buf);
    memset(tmp_buf, '\0', sizeof(tmp_buf));

    return 0;
}

static int construct_sa_data(char *buf, const char *ipv4, const unsigned short port)
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

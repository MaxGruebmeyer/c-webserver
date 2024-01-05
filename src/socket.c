#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "socket.h"

static int get_ipv4_bytes(char *buf, const char *ipv4);
static void get_port_bytes(char *buf, const unsigned short port);

int construct_sockaddr(struct sockaddr *addr, const unsigned addrlen, const char *ipv4, const unsigned short port)
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

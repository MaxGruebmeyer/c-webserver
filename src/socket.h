#ifndef SOCKET_HEADERS

#define SOCKET_HEADERS

/* TODO (GM): Rename these once everything works! */
#define AF_INET_IPv4 2
#define SOCK_STREAM_TCP 1

struct ipv4_sockaddr {
    short int sa_family;

    /* TODO (GM): Replace this with e.g. 7, since that's enough for ipv4? */
    char sa_data[14];
};

#endif

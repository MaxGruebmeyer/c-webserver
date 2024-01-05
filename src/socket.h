#ifndef SOCKET_HEADERS

#define SOCKET_HEADERS

/* TODO (GM): Rename these once everything works! */
#define AF_INET 2
#define SOCK_STREAM 1

struct sockaddr {
    short int sa_family;
    char sa_data[14];
};

int construct_sockaddr(struct sockaddr *addr, const unsigned addrlen, const char *ipv4, const unsigned short port);

#endif

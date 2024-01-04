#ifndef SOCKET_HEADERS

#define SOCKET_HEADERS

/* TODO (GM): Rename these once everything works! */
#define AF_INET 2
#define SOCK_STREAM 1

struct sockaddr {
    short int sa_family;
    char sa_data[14];
};

#endif

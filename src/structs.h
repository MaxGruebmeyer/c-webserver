#ifndef STRUCTS_HEADERS

#define STRUCTS_HEADERS

/* TODO (GM): Leverage this to improve the conversion functions! */
struct sockaddr {
    short int sa_family;
    union {
        struct {
            char port[2];
            char ip[4];
            char sa_zero[8];
        } addr;
        char data[14];
    } sa_data;
};

#endif

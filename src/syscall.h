#ifndef SYSCALL_HEADERS

#define SYSCALL_HEADERS

#include "socket.h"

/* Obviously these numbers are also platform dependant and only work for x86_64 Bit Linux. */
/* Even x86_32 Bit Linux might use different syscall numbers. */
#define CLOSE_SYSCALL_NO 3
#define SOCKET_SYSCALL_NO 41
#define CONNECT_SYSCALL_NO 42
#define ACCEPT_SYSCALL_NO 43
#define SENDTO_SYSCALL_NO 44
#define RECVFROM_SYSCALL_NO 45
#define BIND_SYSCALL_NO 49
#define LISTEN_SYSCALL_NO 50

long syscall(long sysno, ...);

/* TODO (GM): These as macros? */
long close(const long sockfd);
long socket(const int communcation_domain, const int type);
long connect(int sockfd, struct sockaddr *addr, const long addrlen);
long accept(const long sockfd);
long sendto(const long sockfd, char *msg, const long msglen);
long recvfrom(const long sockfd, char *msgptr, const long max_msgsize);
long shutdown();
long bind(const long sockfd, struct sockaddr *addr, const long addrlen);
long listen(const long sockfd, const int backlog_size);

#endif

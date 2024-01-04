#ifndef SYSCALL_HEADERS

#define SYSCALL_HEADERS

#define CLOSE_SYSCALL_NO 3
#define SOCKET_SYSCALL_NO 41
#define CONNECT_SYSCALL_NO 42
#define ACCEPT_SYSCALL_NO 43
#define SENDTO_SYSCALL_NO 44
#define RECVFROM_SYSCALL_NO 45
#define SHUTDOWN_SYSCALL_NO 48
#define BIND_SYSCALL_NO 49
#define LISTEN_SYSCALL_NO 50

int my_syscall(const int syscall_no, int *arg1, int *arg2, int *arg3);

#endif

#ifndef SYSCALL_HEADERS

#define SYSCALL_HEADERS

/* Obviously these numbers are also platform dependant and only work for x86_64 Bit Linux. */
/* Even x86_32 Bit Linux might use different syscall numbers. */
#define CLOSE_SYSCALL_NO 3
#define SOCKET_SYSCALL_NO 41
#define CONNECT_SYSCALL_NO 42
#define ACCEPT_SYSCALL_NO 43
#define SENDTO_SYSCALL_NO 44
#define RECVFROM_SYSCALL_NO 45
#define SHUTDOWN_SYSCALL_NO 48
#define BIND_SYSCALL_NO 49
#define LISTEN_SYSCALL_NO 50

long syscall(long sysno, ...);

#endif

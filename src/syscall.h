#ifndef SYSCALL_HEADERS

#define SYSCALL_HEADERS

#include <netinet/in.h>

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

#define FORK_SYSCALL_NO 57
#define WAIT4_SYSCALL_NO 61
#define KILL_SYSCALL_NO 62

long syscall(long sysno, ...);

int shutdown();

int fork();
int wait4(const int pid);
int wait4_opts(const int pid, int *wstatus, int options);
int kill(const int pid, const int signal);

#endif

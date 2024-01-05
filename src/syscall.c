#include <stdlib.h>

/* Obviously this is hugely OS and platform dependant and only works on x86_64 Linux systems. */
/* For other OS the registers might differ and for x86_32 systems as well. */
/* ARM architectures have not been tested with this. */
#define SYSCALL_NO_REGISTER "rax"
#define SYSCALL_ARG1_REGISTER "rdi"
#define SYSCALL_ARG2_REGISTER "rsi"
#define SYSCALL_ARG3_REGISTER "rdx"

/* This register is only correct for user space. */
/* Kernel space uses %r10 instead of %rcx. */
#define SYSCALL_ARG4_REGISTER "rcx"
#define SYSCALL_ARG5_REGISTER "r8"
#define SYSCALL_ARG6_REGISTER "r9"

/* These need to be longs since we're on a 64-Bit system as specified by the registers */
/* For a 32-Bit system you would need to use other registers. */
long int syscall(long sysno, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6)
{
    register long _syscall_no asm(SYSCALL_NO_REGISTER) = sysno;
    register long _arg1 asm(SYSCALL_ARG1_REGISTER) = arg1;
    register long _arg2 asm(SYSCALL_ARG2_REGISTER) = arg2;
    register long _arg3 asm(SYSCALL_ARG3_REGISTER) = arg3;
    register long _arg4 asm(SYSCALL_ARG4_REGISTER) = arg4;
    register long _arg5 asm(SYSCALL_ARG5_REGISTER) = arg5;
    register long _arg6 asm(SYSCALL_ARG6_REGISTER) = arg6;

    asm("syscall");

    /* SYSCALL_NO_REGISTER contains return value */
    return _syscall_no;
}

int main(void)
{
    syscall(1, 1, (long)"Hello, world!\n", 15, (long)NULL, (long)NULL, (long)NULL);

    return 0;
}

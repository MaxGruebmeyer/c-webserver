#include <stdlib.h>
#include <stdarg.h>

#include "syscall.h"

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

static long syscall_fixed(long sysno, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6);

long syscall(long sysno, ...)
{
    long arg1, arg2, arg3, arg4, arg5, arg6;

    va_list ap;
    va_start(ap, sysno);

    arg1 = va_arg(ap, long);
    arg2 = va_arg(ap, long);
    arg3 = va_arg(ap, long);
    arg4 = va_arg(ap, long);
    arg5 = va_arg(ap, long);
    arg6 = va_arg(ap, long);

    va_end(ap);

    return syscall_fixed(sysno, arg1, arg2, arg3, arg4, arg5, arg6);
}

int main(void)
{
    syscall(1, 1, (long)"Hello, world!\n", 15, (long)NULL, (long)NULL, (long)NULL);

    return 0;
}

/* Disable "-Wunused-variable" for the following function. */
/* The variables are used, just in Assembly and not in C. */
#pragma GCC diagnostic ignored "-Wunused-variable"

/* These need to be longs since we're on a 64-Bit system as specified by the registers */
/* For a 32-Bit system you would need to use other registers. */
static long syscall_fixed(long sysno, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6)
{
    register long _syscall_no __asm__(SYSCALL_NO_REGISTER) = sysno;
    register long _arg1 __asm__(SYSCALL_ARG1_REGISTER) = arg1;
    register long _arg2 __asm__(SYSCALL_ARG2_REGISTER) = arg2;
    register long _arg3 __asm__(SYSCALL_ARG3_REGISTER) = arg3;
    register long _arg4 __asm__(SYSCALL_ARG4_REGISTER) = arg4;
    register long _arg5 __asm__(SYSCALL_ARG5_REGISTER) = arg5;
    register long _arg6 __asm__(SYSCALL_ARG6_REGISTER) = arg6;

    __asm__("syscall");

    /* SYSCALL_NO_REGISTER contains return value */
    return _syscall_no;
}

#pragma GCC diagnostic warning "-Wunused-variable"

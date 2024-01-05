#include <stdlib.h>

/* These need to be longs since we're on a 64-Bit system as specified by the registers */
long int syscall(long sysno, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6)
{
    /* Obviously this is hugely platform dependant and only works on x86_64 Linux systems */
    /* Furthermore this only works from user space since kernel uses different register. */
    /* User space uses the registers %rdi, %rsi, %rdx, %rcx, %r8 and %r9 */
    /* Kernel space uses the registers %rdi, %rsi, %rdx, %r10, %r8 and %r9 */
    register long _syscall_no asm("rax") = sysno;
    register long _arg1 asm("rdi") = arg1;
    register long _arg2 asm("rsi") = arg2;
    register long _arg3 asm("rdx") = arg3;
    register long _arg4 asm("rcx") = arg4;
    register long _arg5 asm("r8") = arg5;
    register long _arg6 asm("r9") = arg6;

    asm("syscall");

    /* %rax contains return value */
    return _syscall_no;
}

int main(void)
{
    syscall(1, 1, (long)"Hello, world!\n", 15, (long)NULL, (long)NULL, (long)NULL);

    return 0;
}

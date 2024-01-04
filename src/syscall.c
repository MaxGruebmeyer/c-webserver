#include <stdio.h>
#include <unistd.h>

/* int main(void)
{
    register int syscall_no asm("rax") = 1;
    register int arg1 asm("rdi") = 0;
    register char* arg2 asm("rsi") = "Hello, world!\n";
    register int arg3 asm("rdx") = 14;
    asm("syscall");

    TODO (GM): Why does this print need to be added?
    printf("Return: %s\n", arg2);
    return 0;
} */

int my_syscall(const int syscall_no, int *arg1, int *arg2, int *arg3)
{
    register int _syscall_no asm("rax") = syscall_no;
    register int *_arg1 asm("rdi") = arg1;
    register int *_arg2 asm("rsi") = arg2;
    register int *_arg3 asm("rdx") = arg3;
    asm("syscall");

    return 0;
}

int main(void)
{
    syscall(1, 1, "Hello, world!\n", 15);
    return 0;
}

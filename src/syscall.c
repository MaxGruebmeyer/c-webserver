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

int main(void)
{
    syscall(1, 1, "Hello, world!\n", 15);
    return 0;
}

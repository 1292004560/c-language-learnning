#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_val = 6;

char buf[] = "a write to stdout\n";

int main(void)
{
    int var;
    pid_t pid;

    var = 88;

    if (write(STDOUT_FILENO, buf, sizeof(buf) - 1) != sizeof(buf) - 1)
    {
        printf("write error");
    }

    printf("before fork\n");
    fflush(stdin);

    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
    }
    else if (pid == 0)
    {
        global_val ++;
        var++;
    }else {
        sleep(2);
    }

    printf("pid = %ld, glob = %d, var = %d \n", (long)getpid(), global_val, var);

    exit(1);
}
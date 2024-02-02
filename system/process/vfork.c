#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int globalval = 6;

int main(void)
{
    int var;
    pid_t pid;

    var = 88;

    printf("before vfork\n");

    if ((pid = vfork()) < 0)
    {

        printf("vfork error !");
    }
    else if (pid == 0)
    {
        globalval++;
        var ++;
        _exit(0);
    }

    printf("pid = %ld, glob = %d, var = %d\n", (long)getpid(), globalval, var);

    exit(0);
}
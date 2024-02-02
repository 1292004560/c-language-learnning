#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        printf("fork error");
    }
    else if (pid == 0)
    {
        if ((pid = fork()) < 0)
            printf("fork error");
        else if (pid > 0)
        {
            exit(0);
        }

        sleep(2);
        printf("second child, parent pid = %ld\n", (long)getpid());
        exit(0);
        
    }

    if (waitpid(pid, NULL, 0) != pid)
        printf("waitpid() error");
    
    exit(0);
}
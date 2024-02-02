#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

char * env_init[] = {"USER=unknow", "PATH=/tmp", NULL};

int main(void)
{
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        printf("fork error");
    }
    else if (pid == 0)
    {
        if (execle("/root/c-language-learnning/system/process/echoall", "echoall", "myarg1", "MY ARG2", (char *)0, env_init) < 0)
            printf("execle error");
    }
    if (waitpid(pid, NULL, 0) < 0)
        printf("wait error");
    
    if ((pid = fork()) < 0)
    {
        printf("frok error");
    }
    else if (pid == 0)
    {
        if (execlp("ls", "al", "/", (char *)0) < 0)
            printf("execlp error");
    }

     if (waitpid(pid, NULL, 0) < 0)
        printf("wait error\n");
    
    exit(1);
}
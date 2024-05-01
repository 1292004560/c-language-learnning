#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {

  int pipefd[2]; // 0 read 1 write

  pid_t pid;

  int ret = pipe(pipefd);

  if (ret == -1) {
    perror("pipe error !");
    exit(1);
  }

  pid = fork();

  if (pid == -1) {
    perror("fork error!");
    exit(1);
  } else if (pid == 0) {

    close(pipefd[1]);
    char buf[1024];

    printf("child = %d \n", getpid());
    ret = read(pipefd[0], buf, sizeof(buf));
    write(STDOUT_FILENO, buf, ret);
  } else {

    sleep(1);
    close(pipefd[0]);
    printf("parent = %d\n", getpid());
    write(pipefd[1], "hello pipe\n", 11);
    wait(NULL); // recycle child process
  }

  printf("finished ! %d\n", getpid());

  return 0;
}

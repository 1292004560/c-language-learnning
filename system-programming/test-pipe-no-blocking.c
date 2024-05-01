#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {

  int pipefd[2];

  pid_t pid;

  int ret = pipe(pipefd);

  if (ret == -1) {

    perror("pipe error");
    exit(1);
  }

  pid = fork();

  if (pid == -1) {
    perror("fork error !");
    exit(1);
  } else if (pid == 0) {
    close(pipefd[1]);

    int flags = fcntl(pipefd[0], F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(pipefd[0], F_SETFL, flags);

    char buf[1024];
  tryagain:
    ret = read(pipefd[0], buf, sizeof(buf));
    if (ret == -1) {

      // 如果数据没到达 会返回EAGAIN
      if (errno == EAGAIN) {

        write(STDOUT_FILENO, "try again!\n", 11);
        sleep(1);
        goto tryagain;
      } else {
        perror("read error");
        exit(1);
      }
    }
    write(STDOUT_FILENO, buf, ret);
    close(pipefd[0]);
  } else {
    sleep(5);
    close(pipefd[0]);
    write(pipefd[1], "hello pipe\n", 11);
    wait(NULL);
    close(pipefd[1]);
  }

  return 0;
}

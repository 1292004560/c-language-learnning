#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {

  int fd = open("./mmap.c", O_RDONLY);

  int fx = open("newhello.txt", O_CREAT | O_WRONLY, 0644);

  char buf[1024];

  int n;

  while ((n = read(fd, buf, sizeof(buf)))) {

    int ret = write(fx, buf, n);

    if (ret == -1) {
      perror("write error");
      exit(1);
    }

    printf("write bytes : %d\n", n);
  }

  close(fd);
  close(fx);
  return 0;
}

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define P_FIFO "/tmp/p_fifo"

int main(int argc, char *argv[]) {

  int fd;

  if (argc < 2)
    printf("please input the write data. \n");

  fd = open(P_FIFO, O_WRONLY | O_NONBLOCK);

  if (fd == -1) {
    printf("open failed!\n");
    return 0;
  } else {
    printf("open success !\n");
  }

  write(fd, argv[1], 100);
  close(fd);
  return EXIT_SUCCESS;
}

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  int fd = open("aa.txt", O_CREAT | O_RDWR);

  int file_size = lseek(fd, 0, SEEK_END);
  printf("file size = %d\n", file_size);

  int total_size = lseek(fd, 2000, SEEK_END);
  printf("total size = %d\n", total_size);

  write(fd, "a", 1);

  return EXIT_SUCCESS;
}

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {

  char *p = NULL;
  int fd = -1;

  fd = open("temp", O_RDWR | O_CREAT | O_TRUNC, 0644);

  if (-1 == fd) {
    printf("open error\n");
    return -1;
  }

  lseek(fd, 1023, SEEK_END);

  write(fd, "\0", 1);

  p = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (p == MAP_FAILED) {
    printf("mmap error \n");
    close(fd);
    return -1;
  }

  memcpy(p, "hello world", sizeof("hello world"));

  printf("p = %s \n", p);

  if (munmap(p, 1024) == -1)
    exit(-1);

  close(fd);

  return EXIT_SUCCESS;
}

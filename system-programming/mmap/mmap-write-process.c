#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct student {
  int id;
  char name[256];
  int age;
};

int main(int argc, char *argv[]) {

  int fd;

  struct student stu = {0, "zhangshang", 18};
  struct student *p;

  fd = open("temp", O_RDWR | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    perror("open error");
    exit(1);
  }
  ftruncate(fd, sizeof(stu));
  p = mmap(NULL, sizeof(stu), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (p == MAP_FAILED) {
    perror("mmap error");
    exit(1);
  }
  close(fd);

  while (1) {
    // 循环写
    memcpy(p, &stu, sizeof(stu));
    stu.id++;
    sleep(3);
  }
  if (-1 == munmap(p, sizeof(stu))) {
    perror("munmap error");
    exit(1);
  }
  return EXIT_SUCCESS;
}

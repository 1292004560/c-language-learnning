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

  fd = open("temp", O_RDONLY, 0644);
  if (fd < 0) {
    perror("open error");
    exit(1);
  }
  p = mmap(NULL, sizeof(stu), PROT_READ, MAP_SHARED, fd, 0);

  if (p == MAP_FAILED) {
    perror("mmap error");
    exit(1);
  }

  close(fd);

  while (1) {
    // 循环读
    printf("stu.id = %d, stu.name = %s, stu.age = %d\n", p->id, p->name,
           p->age);
    sleep(3);
  }

  if (-1 == munmap(p, sizeof(stu))) {
    perror("munmap error");
    exit(1);
  }

  return EXIT_SUCCESS;
}

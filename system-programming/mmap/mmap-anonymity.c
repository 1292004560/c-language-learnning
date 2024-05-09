#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// 全局变量 var
int var = 100;

int main(int argc, char *argv[]) {

  int *p;

  pid_t pid;

  int ret = 0;

  int fd;

  // 打开一个文件
  fd = open("temp", O_RDWR | O_TRUNC | O_CREAT, 0644);
  if (fd < 0) {
    perror("open error");
    exit(1);
  }
  // TODO 添加了这句删除文件

  ret = unlink("temp");

  p = (int *)mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (p == MAP_FAILED) {
    perror("mmap error");
    exit(1);
  }

  ftruncate(fd, 4);
  // 关闭fd，mmap创建成功后就可以关闭了，因为直接使用地址了，不需要fd了
  close(fd);

  // fork一个进程
  pid = fork();

  if (pid == 0) {
    *p = 2000;
    var = 1000;
    printf("child *p = %d, var = %d\n", *p, var);
  } else {
    sleep(1); // 休眠一秒，让子进程先执行
    printf("parent *p = %d, var = %d\n", *p, var);
    wait(NULL); // 回收子进程
    // 释放共享内存
    if (munmap(p, 4) == -1) {
      perror("munmap error");
      exit(1);
    }
  }

  return EXIT_SUCCESS;
}

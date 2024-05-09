#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
/* 首先规定一下该命令的参数 ：
 * mycopy 源文件地址 目标文件地址
 *
 * 可能需要的函数：open\read\write\close
 * 基本流程：打开源文件，读取内容，写到另一个文件，关闭文件
 * 核心方法：通过mmap(最快的进程间通信方法)
 * 如何获取文件的大小：
 *
 */

unsigned long get_file_byte_num(const char *filename) {
  FILE *fp = fopen(filename, "r");
  fseek(fp, 0, SEEK_END);
  return ftell(fp);
}

int main(int argc, char *argv[]) {

  // 首先解析输入的参数得到源文件和目标文件
  if (argc < 3) {
    perror("参数输入不足");
  }
  int source_file_length = strlen(argv[1]); // 这个是源文件的名的长度
  int target_file_length = strlen(argv[2]); // 这个是目标文件名的长度

  char *source_filename =
      (char *)malloc(sizeof(char) * (source_file_length + 1));
  char *target_filename =
      (char *)malloc(sizeof(char) * (target_file_length + 1));

  strcpy(source_filename, argv[1]);

  strcpy(target_filename, argv[2]);

  // 打开目标文件
  // 计算源文件的大小（字节数）
  unsigned long byte_num = get_file_byte_num(source_filename);
  printf("文件的长度为%ld字节\n", byte_num);
  //--------建立 mmap 映射区 --------------
  // 获取被复制文件的文件描述符
  int source_fd = open(source_filename, O_RDWR | O_CREAT, 0644);
  int target_fd = open(target_filename, O_RDWR | O_CREAT, 0644);

  ftruncate(target_fd, byte_num);

  char *source_mmap = (char *)mmap(NULL, byte_num, PROT_WRITE | PROT_READ,
                                   MAP_SHARED, source_fd, 0);

  char *target_mmap = (char *)mmap(NULL, byte_num, PROT_WRITE | PROT_READ,
                                   MAP_SHARED, target_fd, 0);

  if (source_mmap == MAP_FAILED)
    perror("source_mmap err");

  if (target_mmap == MAP_FAILED)
    perror("target_mmap err");

  close(source_fd); // 内存映射区建立之后，就可以关闭文件描述符
  close(target_fd);

  memcpy(target_mmap, source_mmap, byte_num);

  // 回收子进程，等待拷贝结束
  munmap(source_mmap, byte_num);
  munmap(target_mmap, byte_num);
  free(source_filename);
  free(target_filename);
  return EXIT_SUCCESS;
}

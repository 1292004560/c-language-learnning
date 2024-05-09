#include <fcntl.h>
#include <malloc.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
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
// 获取文件的字节数
unsigned long get_file_byte_num(const char *filename) {
  FILE *fp = fopen(filename, "r");
  fseek(fp, 0, SEEK_END);
  return ftell(fp);
}

// 信号处理函数
void subprocess_signal_handle(int a) {
  pid_t pid;
  while ((pid = waitpid(0, NULL, WNOHANG)) > 0) {
    printf("回收到一个子进程%d\n", pid);
  }
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
  FILE *target_fp = fopen(target_filename, "w"); // 不存在则创建
  // 计算源文件的大小（字节数）
  unsigned long byte_num = get_file_byte_num(source_filename);
  printf("文件的长度为%ld字节\n", byte_num);

  //--------建立 mmap 映射区 --------------
  // 获取被复制文件的文件描述符
  int source_fd = open(source_filename, O_RDWR | O_CREAT, 0644);
  int target_fd = open(target_filename, O_RDWR | O_CREAT, 0644);
  ftruncate(target_fd, byte_num); // 将tfd指向的文件的大小改变为byte_num

  char *mmap_source = (char *)mmap(NULL, byte_num, PROT_WRITE | PROT_READ,
                                   MAP_SHARED, source_fd, 0);
  char *mmap_source_tmp = mmap_source; // 备份共享内存入口地址
  if (mmap_source == MAP_FAILED)
    perror("mmap_source MAP_FAILED");
  char *mmap_target =
      (char *)mmap(NULL, byte_num, PROT_WRITE, MAP_SHARED, target_fd, 0);
  char *mmap_target_tmp = mmap_target;
  if (mmap_target == MAP_FAILED)
    perror("mmap_target err");

  close(source_fd); // 内存映射区建立之后，就可以关闭文件描述符
  close(target_fd);

  // 父子进程之间就可以通过这个mmap_source指针进行通信了
  // 规定进程数量为5mZ    const int num_process = 5;
  // 根据进程数量，计算每个进程需要拷贝的字节数，
  // 最后一个进程拷贝剩余的字节数。
  const int num_process = 5;
  const unsigned long each_process_byte = byte_num / num_process;
  // 由于可能无法整除，因此最后一个进程，需要多复制一些
  const unsigned long last_process_byte =
      byte_num - each_process_byte * (num_process - 1);

  // 屏蔽sigchld信号的系统默认处理方式
  sigset_t set;
  sigemptyset(&set); // 初始化一下set
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, NULL); // 屏蔽

  // 循环创建子进程
  int i;
  pid_t pid;
  for (i = 0; i < num_process - 1; ++i) {
    if ((pid = fork()) == -1)
      perror("fork error");
    if (pid == 0)
      break;
  }

  // -------具体的拷贝过程---------
  if (i == num_process - 1) // 父进程
  {
    // 设置信号捕捉和解除屏蔽
    struct sigaction act;
    act.sa_handler = subprocess_signal_handle;
    sigemptyset(&act.sa_mask);            // 处理期间不屏蔽其他信号
    sigaction(SIGCHLD, &act, NULL);       // 启动信号捕获
    sigprocmask(SIG_UNBLOCK, &set, NULL); // 解除SIGCHLD的屏蔽

    memcpy(mmap_target_tmp + each_process_byte * i,
           mmap_source_tmp + each_process_byte * i, last_process_byte);

  } else {
    memcpy(mmap_target + each_process_byte * i,
           mmap_source_tmp + each_process_byte * i, each_process_byte);
  }
  // 回收子进程，等待拷贝结束

  munmap(mmap_source, byte_num);
  munmap(mmap_target, byte_num);
  free(source_filename);
  free(target_filename);
  return 0;
  return EXIT_SUCCESS;
}

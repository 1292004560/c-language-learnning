#include <printf.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

// 注意这个magic数字的由来。这是我基于/proc/$pid/maps文件计算出来的：
// 计算方法：用stack头减去heap尾即可，然后根据OOM信息手动修改
#define CNT 8638055936

char *data;
int cnt = 0;

int main(int argc, char *argv[]) {
  long i, j = 0;
  // base就是heap尾的大致位置。
  unsigned long st1, base = 0x7f510721e000;
  pid_t pid;
  int ps = sysconf(_SC_PAGE_SIZE);

  // 由于我要fix映射，所以需要页面对齐。
  base = ((unsigned long)base & 0xfffffffffffff000);

  // 写时复制备用
  st1 = base;

  // 循环构建稀疏地址空间，即将CNT/ps/16个页面均匀摊到heap和stack之间的所有区域。
  // 受限于系统内存有限，CNT的值可以修改，我就改小了，因为我的虚拟机太矬。
  for (i = 0; i < CNT; i += ps * ps / 16) {
    // FIX映射，PRIVATE映射
    data = mmap((void *)base, ps - 1, PROT_READ | PROT_WRITE,
                MAP_ANON | MAP_PRIVATE | MAP_FIXED, -1, 0);
    // 为展示写时复制的开销，父进程的稀疏页面需要在内存中
    mlock(data, ps - 1);
    base += ps * ps / 16;
    cnt++;
  }
  printf("mmap:%p %lx   cnt:%d\n", data, base, cnt);

  /*slabtop; cat /proc/meminfo|grep Slab; cat /proc/meminfo|grep SUnreclaim;
   * free -m 后回车*/
  printf("请观察fork之前的内存用量！\n");
  printf("请注意稀疏mmap对页表内存占用的影响！\n");
  printf("敲任意键执行fork！\n\n");
  getchar();

  if ((pid = fork()) < 0) {
    printf("create failed\n");
    exit(1);
  } else if (pid == 0) {
    /*slabtop; cat /proc/meminfo|grep Slab; cat /proc/meminfo|grep SUnreclaim;
     * free -m 后回车*/
    printf("请观察fork之后，exec之前的内存用量！\n");
    printf("敲任意键执行exec！\n\n");
    getchar();
    printf("现在请观察exec之后的内存用量！\n");
    if (execl("/usr/bin/echo", "echo", "skinshoe", NULL) < 0) {
      perror("error on exec");
    }
  } else {
  }
  // 写稀疏内存！
  // 如果这个发生在子进程exec之前，将会导致不必要的写时复制。
  for (i = 0; i < CNT; i += ps * ps / 16) {
    *(char *)st1 = 122;
    st1 += ps * ps / 16;
  }
  sleep(1000);
  return 0;
}

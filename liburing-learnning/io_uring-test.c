#define _GNU_SOURCE
#include "liburing.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define QD 4

int main(int argc, char *argv[]) {

  struct io_uring ring;
  int i, fd, ret, pending, done;
  struct io_uring_cqe *cqe;
  struct iovec *iovecs;

  struct stat sb;
  ssize_t fsize;
  off_t offset;
  void *buf;

  if (argc < 2) {
    printf("%s : file \n", argv[0]);
    return 1;
  }
  // 1. 初始化一个 io_uring 实例
  ret = io_uring_queue_init(QD, &ring, 0); // 队列长度    io_uring 实例 flags，0
                                           // 表示默认配置，例如使用中断驱动模式

  if (ret < 0) {
    fprintf(stderr, "queue_init : %s \n", strerror(-ret));
    return 1;
  }
  // 2. 打开输入文件，注意这里指定了 O_DIRECT flag，内核轮询模式需要这个 flag
  fd = open(argv[1], O_RDONLY | O_DIRECT);
  if (fd < 0) {
    perror("fstat");
    return 1;
  }
  // 获取文件信息，例如文件长度，后面会用到
  if (fstat(fd, &sb) < 0) {
    perror("fstat");
    return 1;
  }

  fsize = 0; // 程序的最大读取长度
             // 3. 初始化 4 个读缓冲区
  iovecs = calloc(QD, sizeof(struct iovec));

  for (i = 0; i < QD; i++) {
    if (posix_memalign(&buf, 4096, 4096))
      return 1;

    iovecs[i].iov_base = buf;
    iovecs[i].iov_len = 4096;
    fsize += 4096;
  }
  offset = 0;
  i = 0;
  // 4. 依次准备 4 个 SQE 读请求，指定将随后读入的数据写入 iovecs
  struct io_uring_sqe *sqe;
  do {
    sqe = io_uring_get_sqe(&ring); // 获取可用 SQE
    if (!sqe) {
      break;
    }
    // 用这个 SQE 准备一个待提交的 read 操作
    // 从 fd 打开的文件中读取数据
    // iovec 地址，读到的数据写入 iovec 缓冲区
    // iovec 数量
    // 读取操作的起始地址偏移量
    // 更新偏移量，下次使用
    io_uring_prep_readv(sqe, fd, &iovecs[i], 1, offset);
    offset += iovecs[i].iov_len;
    i++;
    if (offset >= sb.st_size) // 如果超出了文件大小，停止准备后面的 SQE
      break;
  } while (1);
  // 5. 提交 SQE 读请求
  ret = io_uring_submit(&ring); // 4 个 SQE 一次提交，返回提交成功的 SQE 数量
  if (ret < 0) {
    fprintf(stderr, "io_uring_submit : %s \n", strerror(-ret));
    return 1;
  } else if (ret != i) {
    fprintf(stderr, "io_uring_submit submitted less %d\n", ret);
    return 1;
  }

  // 6. 等待读请求完成（CQE）
  done = 0;
  pending = ret;
  fsize = 0;
  for (i = 0; i < pending; i++) {
    ret = io_uring_wait_cqe(&ring, &cqe); // 等待系统返回一个读完成事件
    if (ret < 0) {
      fprintf(stderr, "io_uring_wait_cqe : %s\n", strerror(-ret));
      return 1;
    }
    done++;
    ret = 0;

    if (cqe->res != 4096 && cqe->res + fsize != sb.st_size) {
      fprintf(stderr, "ret=%d, wanted 4096\n", cqe->res);
      return 1;
    }
    fsize += cqe->res;
    io_uring_cqe_seen(&ring, cqe); // 更新 io_uring 实例的完成队列
    if (ret) {
      break;
    }
  }

  printf("submitted = %d, completed = %d, bytes=%lu\n", pending, done,
         (unsigned long)fsize);
  close(fd);
  io_uring_queue_exit(&ring);
  return EXIT_SUCCESS;
}

#include <fcntl.h>
#include <linux/fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/uio.h>

#define BLOCK_SZ 4096

/*
 * 返回传入文件描述符的文件的大小。
 * 正确处理常规文件和块设备。
 * */
off_t get_file_size(int fd) {

  struct stat st;

  if (fstat(fd, &st) < 0) {

    perror("fstat error");
    return -1;
  }

  if (S_ISBLK(st.st_mode)) {
    unsigned long long bytes;

    if (ioctl(fd, BLKGETSIZE64, &bytes) != 0) {
      perror("ioctl error");
      return -1;
    }
    return bytes;
  } else if (S_ISREG(st.st_mode)) {
    return st.st_size;
  }

  return -1;
}

/*
 * 输出长度为 len 的字符串到 stdout
 * 我们在这里使用缓冲输出以提高效率，
 * 因为我们需要逐个字符地输出。
 * */
void out_to_console(char *buf, int len) {
  while (len--) {
    fputc(*buf++, stdout);
  }
}

int read_and_print_file(char *filename) {
  struct iovec *iovecs;

  int filefd = open(filename, O_RDONLY);
  if (filefd < 0) {
    perror("open error");
    return 1;
  }

  off_t file_size = get_file_size(filefd);

  off_t bytes_remaining = file_size;

  int blocks = (int)file_size / BLOCK_SZ;

  if (file_size % BLOCK_SZ)
    blocks++;

  iovecs = malloc(sizeof(struct iovec) * blocks);

  int current_block = 0;

  /*
   * 对于我们正在读取的文件，分配足够的块来容纳
   * 文件数据。每个块都在一个 iovec 结构中描述，
   * 它作为 iovecs 数组的一部分传递给 readv。
   * */
  while (bytes_remaining) {
    off_t bytes_to_read = bytes_remaining;

    if (bytes_to_read > BLOCK_SZ)
      bytes_to_read = BLOCK_SZ;

    void *buf;

    if (posix_memalign(&buf, BLOCK_SZ, BLOCK_SZ)) {
      perror("posix_memalign error");
      return 1;
    }
    iovecs[current_block].iov_base = buf;
    iovecs[current_block].iov_len = bytes_to_read;
    current_block++;
    bytes_remaining -= bytes_to_read;
  }

  /*
   * readv() 调用将阻塞，直到所有 iovec 缓冲区被填满
   * 文件数据。一旦它返回，我们应该能够从 iovecs 访问文件数据
   * 并在控制台上打印它们。
   * */
  int ret = readv(filefd, iovecs, blocks);
  if (ret < 0) {
    perror("readv error");
    return 1;
  }

  for (int i = 0; i < blocks; i++) {
    out_to_console(iovecs[i].iov_base, iovecs[i].iov_len);
  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename1> [<filename2> ...]\n", argv[0]);
    return 1;
  }

  for (int i = 1; i < argc; i++) {
    if (read_and_print_file(argv[i])) {
      fprintf(stderr, "Error reading file\n");
      return 1;
    }
  }

  return EXIT_SUCCESS;
}

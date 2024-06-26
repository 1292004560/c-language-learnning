#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

char write_buffer[] = "hello world\n";
char read_buffer[200];

int main(void) {

  int fd;

  char *mmap_address;

  fd = open("data.txt", O_RDWR | O_CREAT);
  if (fd < 0) {
    printf("open failed\n");
    return -1;
  }

  write(fd, write_buffer, strlen(write_buffer) + 1);
  lseek(fd, 0, SEEK_SET);
  read(fd, read_buffer, 100);

  

  printf("read : %s\n", read_buffer);

  mmap_address = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  printf("mmap read : %s\n", mmap_address);

  memcpy(mmap_address + 5, "hello,zhaixue.cc\n", 20);

  printf("mmap read : %s\n", mmap_address);

  munmap(mmap_address, 4096);

  return 0;
}

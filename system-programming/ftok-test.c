#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
int main(int argc, char **argv) {
  struct stat stat1;
  if (argc != 2) {
    printf("usage: ftok < pathname >");
    exit(1);
  }
  stat(argv[1], &stat1);
  printf("st_dev:%lx, st_ino:%lx, key:%x\n", (unsigned long)stat1.st_dev,
         (unsigned long)stat1.st_ino, ftok(argv[1], 0x579));
  printf("st_dev:%lx, st_ino:%lx, key:%x\n", (unsigned long)stat1.st_dev,
         (unsigned long)stat1.st_ino, ftok(argv[1], 0x118));
  printf("st_dev:%lx, st_ino:%lx, key:%x\n", (unsigned long)stat1.st_dev,
         (unsigned long)stat1.st_ino, ftok(argv[1], 0x22));
  printf("st_dev:%lx, st_ino:%lx, key:%x\n", (unsigned long)stat1.st_dev,
         (unsigned long)stat1.st_ino, ftok(argv[1], 0x33));
  exit(0);
}

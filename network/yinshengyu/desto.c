#include <fcntl.h>
#include <stdio.h>
int main(void) {
  FILE *fp;
  int fd = open("data.bat", O_WRONLY | O_CREAT | O_TRUNC);

  open("data.bat", O_CREAT);

  if (fd == -1) {
    fputs("file open error", stdout);
    return -1;
  }

  fp = fdopen(fd, "w");
  fputs("Network C programming \n", fp);
  fclose(fp);
  return 0;
}

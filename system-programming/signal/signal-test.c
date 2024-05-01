#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void do_signal(int sig) { printf("Hi, SIGINT, how do you do !\n"); }

int main(int argc, char *argv[]) {

  if (signal(SIGINT, do_signal) == SIG_ERR) {
    perror("signal error");
    exit(1);
  }

  while (1) {
    printf("-----------------------------------\n");
    sleep(1);
  }

  return EXIT_SUCCESS;
}

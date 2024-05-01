#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

void signal_handler(int sig) {
  int count = 5;

  if (sig == SIGUSR1) {

    while (count > 0) {
      fprintf(stderr, "a");
      sleep(1);
      count--;
    }
  }
  if (sig == SIGUSR2) {

    while (count > 0) {
      fprintf(stderr, "b");
      sleep(1);
      count--;
    }
  }
}

int main(int argc, char *argv[]) {

  signal(SIGUSR1, signal_handler);
  // signal(SIGUSR2, signal_handler);

  struct sigaction action;
  bzero(&action, sizeof(action));
  action.sa_handler = signal_handler;
  sigemptyset(&action.sa_mask);
  sigaddset(&action.sa_mask, SIGUSR1);
  sigaction(SIGUSR2, &action, NULL);

  while (1) {
    pause();
  }

  return EXIT_SUCCESS;
}

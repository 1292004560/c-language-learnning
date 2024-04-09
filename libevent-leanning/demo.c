#include <event.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct event ev;

struct timeval tv;

void timer_cb(int fd, short event, void *arg) {
  printf("timer_cb\n");
  event_add(&ev, &tv);
}

int main(int argc, char *argv[]) {

  struct event_base *base = event_init();
  tv.tv_sec = 1;
  tv.tv_usec = 0;

  event_set(&ev, -1, 0, timer_cb, NULL);
  event_base_set(base, &ev);
  event_add(&ev, &tv);
  event_base_dispatch(base);
  return EXIT_SUCCESS;
}

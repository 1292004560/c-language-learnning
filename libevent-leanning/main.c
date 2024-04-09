#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

  struct event_base *base = event_base_new();
  event_base_dispatch(base);

  return EXIT_SUCCESS;
}

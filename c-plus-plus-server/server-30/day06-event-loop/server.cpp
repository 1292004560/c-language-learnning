#include "src/Server.h"
#include "src/EventLoop.h"

int main(int argc, char *argv[]) {

  EventLoop *loop = new EventLoop();
  Server *server = new Server(loop);
  loop->loop();
  return 0;
}

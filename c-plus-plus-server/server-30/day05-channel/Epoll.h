#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel;
class Epoll {

private:
  int epollfd;
  struct epoll_event *events;

public:
  Epoll();
  ~Epoll();

  void addFileDescriptor(int fd, uint32_t op);

  void updateChannel(Channel *);

  std::vector<Channel *> poll(int timeout = -1);
};

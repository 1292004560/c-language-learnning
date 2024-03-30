#include "Epoll.h"
#include "util.h"
#include <strings.h>
#include <unistd.h>

#define MAX_EVENTS 1000

Epoll::Epoll() : epollfd(-1), events(nullptr) {
  epollfd = epoll_create1(0);

  errif(epollfd == -1, "epoll_create1 error");

  events = new epoll_event[MAX_EVENTS];
  bzero(events, sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll() {
  if (epollfd != -1) {
    close(epollfd);
    epollfd = -1;
  }
  delete[] events;
}

void Epoll::addFileDescriptor(int fd, uint32_t op) {

  struct epoll_event ev;
  bzero(&ev, sizeof(ev));
  ev.data.fd = fd;
  ev.events = op;
  errif(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1,
        "EPOLL_CTL_ADD failed");
}

std::vector<epoll_event> Epoll::poll(int timeout) {
  std::vector<epoll_event> activeEvents;

  int nfds = epoll_wait(epollfd, events, MAX_EVENTS, timeout);
  errif(nfds == -1, "epoll_wait error");
  for (int i = 0; i < nfds; ++i)
    activeEvents.push_back(events[i]);

  return activeEvents;
}

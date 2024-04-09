#include "Epoll.h"
#include "Channel.h"
#include "util.h"
#include <strings.h>
#include <unistd.h>

#define MAX_EVENTS 1000

Epoll::Epoll() : epollfd(-1), events(nullptr) {

  epollfd = epoll_create1(0);
  errif(epollfd == -1, "epoll create error");
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
        "epoll add event error");
}

std::vector<Channel *> Epoll::poll(int timeout) {

  std::vector<Channel *> activeChannels;

  int nfds = epoll_wait(epollfd, events, MAX_EVENTS, timeout);
  errif(nfds == -1, "epoll wait error");

  for (int i = 0; i < nfds; ++i) {
    Channel *channel = (Channel *)events[i].data.ptr;
    channel->setReadEvents(events[i].events);
    activeChannels.push_back(channel);
  }

  return activeChannels;
}

void Epoll::updateChannel(Channel *channel) {
  int fd = channel->getFileDescriptor();

  struct epoll_event ev;
  bzero(&ev, sizeof(ev));
  ev.data.ptr = channel;
  ev.events = channel->getEvents();
  if (!channel->getInEpoll()) {
    errif(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
    channel->setInEpoll();
  } else {
    errif(epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev) == -1,
          "epoll modify error");
  }
}

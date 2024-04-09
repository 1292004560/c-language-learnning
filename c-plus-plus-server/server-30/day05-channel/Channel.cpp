#include "Channel.h"
#include "Epoll.h"

Channel::Channel(Epoll *_epoll, int _fd)
    : epoll(_epoll), fd(_fd), events(0), readEvents(0), inEpoll(false) {}

Channel::~Channel() {}

void Channel::enableReading() {
  events = EPOLLIN | EPOLLET;
  epoll->updateChannel(this);
}

int Channel::getFileDescriptor() { return fd; }

uint32_t Channel::getEvents() { return events; }

uint32_t Channel::getReadEvents() { return readEvents; }

bool Channel::getInEpoll() { return inEpoll; }

void Channel::setInEpoll() { inEpoll = true; }

void Channel::setReadEvents(uint32_t _ev) { readEvents = _ev; }

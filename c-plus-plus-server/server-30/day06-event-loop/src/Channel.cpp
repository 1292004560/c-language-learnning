#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(EventLoop *_loop, int _fd)
    : eventLoop(_loop), fd(_fd), events(0), readEvents(0), inEpoll(false) {}

Channel::~Channel() {}

void Channel::handleEvent() { callback(); }

void Channel::enableReading() {
  events = EPOLLIN | EPOLLET;
  eventLoop->updateChannel(this);
}

int Channel::getFileDescriptor() { return fd; }

uint32_t Channel::getEvents() { return events; }

uint32_t Channel::getReadEvents() { return readEvents; }

bool Channel::getInEpoll() { return inEpoll; }

void Channel::setInEpoll() { inEpoll = true; }

void Channel::setReadEvents(uint32_t _ev) { readEvents = _ev; }

void Channel::setCallback(std::function<void()> _callback) {
  callback = _callback;
}

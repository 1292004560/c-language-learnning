#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include <vector>

EventLoop::EventLoop() : epoll(nullptr), quit(false) { epoll = new Epoll(); }

EventLoop::~EventLoop() { delete epoll; }

void EventLoop::loop() {

  while (!quit) {
    std::vector<Channel *> channels;

    channels = epoll->poll();

    for (auto it = channels.begin(); it != channels.end(); ++it) {
      (*it)->handleEvent();
    }
  }
}

void EventLoop::updateChannel(Channel *channel) {
  epoll->updateChannel(channel);
}

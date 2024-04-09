#pragma once
#include <functional>
#include <sys/epoll.h>

class EventLoop;

class Channel {

private:
  EventLoop *eventLoop;
  int fd;
  uint32_t events;
  uint32_t readEvents;
  bool inEpoll;
  std::function<void()> callback;

public:
  Channel(EventLoop *_eventLoop, int _fd);
  ~Channel();

  void handleEvent();
  void enableReading();

  int getFileDescriptor();
  uint32_t getEvents();
  uint32_t getReadEvents();
  bool getInEpoll();
  void setInEpoll();

  void setReadEvents(uint32_t);
  void setCallback(std::function<void()>);
};

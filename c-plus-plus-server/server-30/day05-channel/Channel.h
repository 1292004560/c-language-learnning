#pragma once
#include <sys/epoll.h>

class Epoll;
class Channel {

private:
  Epoll *epoll;
  int fd;
  uint32_t events;
  uint32_t readEvents;
  bool inEpoll;

public:
  Channel(Epoll *_epoll, int _fd);
  ~Channel();

  void enableReading();

  int getFileDescriptor();

  uint32_t getEvents();
  uint32_t getReadEvents();
  bool getInEpoll();

  void setInEpoll();

  void setReadEvents(uint32_t);
};

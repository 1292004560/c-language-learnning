#pragma once

class EventLoop;
class Socket;

class Server {

private:
  EventLoop *eventLoop;

public:
  Server(EventLoop *);
  ~Server();

  void handleReadEvent(int);
  void newConnection(Socket *serverSocket);
};

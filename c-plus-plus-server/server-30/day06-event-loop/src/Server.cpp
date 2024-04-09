#include "Server.h"
#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"
#include <errno.h>
#include <functional>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#define READ_BUFFER 1024

Server::Server(EventLoop *_loop) : eventLoop(_loop) {

  Socket *serverSock = new Socket();
  InetAddress *serverAddress = new InetAddress("127.0.0.1", 8888);
  serverSock->bind(serverAddress);

  serverSock->listen();
  serverSock->setnonblocking();

  Channel *serverChannel = new Channel(_loop, serverSock->getFileDescriptor());
  std::function<void()> callback =
      std::bind(&Server::newConnection, this, serverSock);
  serverChannel->setCallback(callback);
  serverChannel->enableReading();
}

Server::~Server() {}

void Server::handleReadEvent(int sockfd) {

  char buffer[READ_BUFFER];

  while (true) {
    // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
    bzero(&buffer, sizeof(buffer));
    ssize_t bytes_read = read(sockfd, buffer, sizeof(buffer));

    if (bytes_read > 0) {
      printf("message from client fd %d: %s\n", sockfd, buffer);
      write(sockfd, buffer, sizeof(buffer));
    } else if (bytes_read == -1 && errno == EINTR) {
      // 客户端正常中断、继续读取
      printf("continue reading");
      continue;
    } else if (bytes_read == -1 &&
               ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
      // 非阻塞IO，这个条件表示数据全部读取完毕
      printf("finish reading once, errno: %d\n", errno);
      break;
    } else if (bytes_read == 0) {
      // EOF，客户端断开连接
      printf("EOF, client fd %d disconnected\n", sockfd);
      close(sockfd); // 关闭socket会自动将文件描述符从epoll树上移除
      break;
    }
  }
}

void Server::newConnection(Socket *serverSock) {
  InetAddress *clientAddress = new InetAddress(); // 会发生内存泄露！没有delete
  Socket *clientSock = new Socket(serverSock->accept(clientAddress));
  printf("new client fd %d! IP: %s Port: %d\n", clientSock->getFileDescriptor(),
         inet_ntoa(clientAddress->address.sin_addr),
         ntohs(clientAddress->address.sin_port));
  clientSock->setnonblocking();
  Channel *clientChannel =
      new Channel(eventLoop, clientSock->getFileDescriptor());
  std::function<void()> callback = std::bind(&Server::handleReadEvent, this,
                                             clientSock->getFileDescriptor());
  clientChannel->setCallback(callback);
  clientChannel->enableReading();
}

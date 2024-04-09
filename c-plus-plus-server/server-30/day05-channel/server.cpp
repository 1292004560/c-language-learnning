#include "Channel.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <vector>

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd) {
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}
void handleReadEvent(int);

int main(int argc, char *argv[]) {

  Socket *serverSock = new Socket();
  InetAddress *serverAddress = new InetAddress("127.0.0.1", 8888);
  serverSock->bind(serverAddress);
  serverSock->listen();
  Epoll *epoll = new Epoll();
  serverSock->setnonblocking();
  Channel *serverChannel = new Channel(epoll, serverSock->getFileDescriptor());
  serverChannel->enableReading();

  while (true) {
    std::vector<Channel *> activeChannels = epoll->poll();
    int nfds = activeChannels.size();

    for (int i = 0; i < nfds; i++) {
      int channelfd = activeChannels[i]->getFileDescriptor();
      if (channelfd == serverSock->getFileDescriptor()) {
        InetAddress *clientAddress = new InetAddress();
        Socket *clientSock = new Socket(serverSock->accept(clientAddress));
        printf("new client fd %d! IP: %s Port: %d\n",
               clientSock->getFileDescriptor(),
               inet_ntoa(clientAddress->address.sin_addr),
               ntohs(clientAddress->address.sin_port));
        clientSock->setnonblocking();
        Channel *clientChannel =
            new Channel(epoll, clientSock->getFileDescriptor());
        clientChannel->enableReading();
      } else if (activeChannels[i]->getEvents() & EPOLLIN) {
        handleReadEvent(activeChannels[i]->getFileDescriptor());
      } else {
        printf("something else happened\n");
      }
    }
  }

  delete serverSock;
  delete serverAddress;
  return 0;
}

void handleReadEvent(int sockfd) {
  char buf[READ_BUFFER];
  while (true) {
    // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
    bzero(&buf, sizeof(buf));
    ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
      printf("message from client fd %d: %s\n", sockfd, buf);
      write(sockfd, buf, sizeof(buf));
    } else if (bytes_read == -1 && errno == EINTR) { // 客户端正常中断、继续读取
      printf("continue reading");
      continue;
    } else if (bytes_read == -1 &&
               ((errno == EAGAIN) ||
                (errno ==
                 EWOULDBLOCK))) { // 非阻塞IO，这个条件表示数据全部读取完毕
      printf("finish reading once, errno: %d\n", errno);
      break;
    } else if (bytes_read == 0) { // EOF，客户端断开连接
      printf("EOF, client fd %d disconnected\n", sockfd);
      close(sockfd); // 关闭socket会自动将文件描述符从epoll树上移除
      break;
    }
  }
}

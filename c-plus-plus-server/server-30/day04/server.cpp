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

  Socket *socket = new Socket();

  InetAddress *address = new InetAddress("127.0.0.1", 8888);
  socket->bind(address);
  socket->listen();
  Epoll *epoll = new Epoll();
  socket->setnonblocking();
  epoll->addFileDescriptor(socket->getFileDescriptor(), EPOLLIN | EPOLLET);

  while (true) {
    std::vector<epoll_event> events = epoll->poll();
    int nfds = events.size();
    for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == socket->getFileDescriptor()) {
        InetAddress *client_address = new InetAddress();
        Socket *client_sock = new Socket(socket->accept(client_address));
        printf("new client fd %d! IP: %s Port: %d\n",
               client_sock->getFileDescriptor(),
               inet_ntoa(client_address->addr.sin_addr),
               ntohs(client_address->addr.sin_port));

        client_sock->setnonblocking();
        epoll->addFileDescriptor(client_sock->getFileDescriptor(),
                                 EPOLLIN | EPOLLET);

      } else if (events[i].events & EPOLLIN) {
        handleReadEvent(events[i].data.fd);
      } else {
        printf("something else happened\n");
      }
    }
  }

  delete socket;
  delete address;
  return 0;
}

void handleReadEvent(int sockfd) {

  char buf[READ_BUFFER];
  while (
      true) { // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
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

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVPORT 9527
#define MAXBUF 4096

int setnonblocking(int fd) {
  int opts;
  if ((opts = fcntl(fd, F_GETFL, 0)) == -1) {
    perror("fcntl");
    return -1;
  }

  opts = opts | O_NONBLOCK;
  if ((opts = fcntl(fd, F_SETFL, opts)) == -1) {
    perror("fcntl");
    return -1;
  }

  return 0;
}

int main(void) {
  char buf[MAXBUF];
  struct sockaddr_in servaddr;
  int fd;
  int n, len;

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  servaddr.sin_port = htons(SERVPORT);

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  if (connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    perror("connect");
    exit(1);
  }
  setnonblocking(fd); // 设置成非阻塞
  printf("Connection ok, send data after sleep 10s\n");
  sleep(10); // 睡眠10秒
  send(fd, "hello world", sizeof("hello world") - 1, 0);
  printf("Send ok, client will be closed after sleep 10s\n");
  sleep(10); // 睡眠10秒
  printf("close\n");
  close(fd);
  return 0;
}

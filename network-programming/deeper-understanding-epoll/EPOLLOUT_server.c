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
#define MAXBUF 8192
#define MAXFDS 5000
#define EVENTSIZE 100

char sndMsg[MAXBUF] = {0};

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
  int len, n;

  struct sockaddr_in servaddr;
  int sockfd, listenfd, epollfd, nfds;

  struct epoll_event ev;
  struct epoll_event events[EVENTSIZE];

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERVPORT);

  if ((epollfd = epoll_create(MAXFDS)) == -1) {
    perror("epoll");
    exit(1);
  }

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  if (setnonblocking(listenfd) == -1) {
    perror("setnonblocking");
    exit(1);
  }

  if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    perror("bind");
    exit(1);
  }

  if (listen(listenfd, 10) == -1) {
    perror("listen");
    exit(1);
  }

  // listen fd只注册EPOLLIN事件， EPOLLOUT不需要注册
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = listenfd;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
    perror("epoll_ctl");
    exit(1);
  }

  for (;;) {
    if ((nfds = epoll_wait(epollfd, events, EVENTSIZE, -1)) == -1) {
      perror("epoll_wait");
      exit(1);
    }

    for (n = 0; n < nfds; n++) {
      if (events[n].data.fd == listenfd) {
        while ((sockfd = accept(listenfd, (struct sockaddr *)NULL, NULL)) > 0) {
          if (setnonblocking(sockfd) == -1) {
            perror("setnonblocking");
            exit(1);
          }
          // 新的fd： 采用边缘触发且注册IN、OUT事件
          ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
          ev.data.fd = sockfd;
          if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
            perror("epoll_ctl");
            exit(1);
          } else {
            printf("new socketfd = %d register epoll success\n", sockfd);
          }
        }
        continue;
      }
      printf("Events = 0x%x\n", events[n].events);
      if (events[n].events & (EPOLLIN | EPOLLOUT) & (EPOLLIN | EPOLLOUT)) {
        printf(">> EPOLLIN And EPOLLOUT event， socketfd = %d\n",
               events[n].data.fd);
      } else if (events[n].events & EPOLLIN) {
        printf(">> Only EPOLLIN event， socketfd = %d\n", events[n].data.fd);
      } else if (events[n].events & EPOLLOUT) {
        printf(">> Only EPOLLOUT， socketfd = %d\n", events[n].data.fd);
      }
    }
  }
}

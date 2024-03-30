#include "util.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd) {
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main(int argc, char *argv[]) {

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  errif(sockfd == -1, "socket create error");

  struct sockaddr_in server_addr;

  bzero(&server_addr, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_addr.sin_port = htons(8888);

  errif(bind(sockfd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1,
        "socket bind error");

  errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

  int epollfd = epoll_create1(0);
  errif(epollfd == -1, "epoll_create1 error");

  struct epoll_event events[MAX_EVENTS], ev;

  bzero(&events, sizeof(events));

  bzero(&ev, sizeof(ev));

  ev.data.fd = sockfd;
  ev.events = EPOLLIN | EPOLLET;

  setnonblocking(sockfd);

  epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);

  while (true) {

    int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    errif(nfds == -1, "epoll_wait error");

    for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == sockfd) {
        struct sockaddr_in client_addr;
        bzero(&client_addr, sizeof(client_addr));
        socklen_t client_address_size = sizeof(client_addr);

        int client_sockfd =
            accept(sockfd, (sockaddr *)&client_addr, &client_address_size);
        errif(client_sockfd == -1, "socket accept error");

        printf("new client fd %d! IP : %s Port : %d \n", client_sockfd,
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        bzero(&ev, sizeof(ev));

        ev.data.fd = client_sockfd;
        ev.events = EPOLLIN | EPOLLET;
        setnonblocking(client_sockfd);
        epoll_ctl(epollfd, EPOLL_CTL_ADD, client_sockfd, &ev);

      } else if (events[i].events & EPOLLIN) {
        char buffer[READ_BUFFER];
        while (true) {
          bzero(&buffer, sizeof(buffer));
          ssize_t bytes_read = read(events[i].data.fd, buffer, sizeof(buffer));
          if (bytes_read > 0) {
            printf("message from client fd %d : %s\n", events[i].data.fd,
                   buffer);
            write(events[i].data.fd, buffer, sizeof(buffer));

          } else if (bytes_read == -1 && errno == EINTR) {
            printf("continue reading");
            continue;
          } else if (bytes_read == -1 &&
                     ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {

            printf("finish reading once, errno : %d\n", errno);
            break;
          } else if (bytes_read == 0) {
            printf("EOF, client fd %d disconncted \n", events[i].data.fd);
            close(events[i].data.fd);
            break;
          }
        }
      } else {
        printf("something else happened\n");
      }
    }
  }
  close(sockfd);
  return 0;
}

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
#define MAXBUF 1024

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

int main(int argc, char *argv[]) {

  char buf[MAXBUF];
  struct sockaddr_in server_address;

  int fd;
  int n, len;

  bzero(&server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  server_address.sin_port = htons(SERVPORT);

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  int nSendBuf = 0;
  socklen_t opt_len = sizeof(int);

  nSendBuf = 3072;

  int ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &nSendBuf, sizeof(int));

  if (ret == -1) {
    perror("setsockopt");
  }

  getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &nSendBuf, &opt_len);

  printf("Client recvbuf len = %d\n", nSendBuf);

  if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) ==
      -1) {
    perror("connect");
    exit(1);
  }

  setnonblocking(fd);
  printf("Connection ok, sleep 10s\n");
  sleep(10); // 睡眠10秒保证  服务端缓冲区被打满
  while (1) {
    len = recv(fd, buf, 256, 0);

    if (len > 0) {
      n += len;
    } else if (len == -1 && errno == EAGAIN) {
      printf("EAGAIN, Has recv n = %d\n", n);
      sleep(3);
    } else {
      printf("Recv failed.\n");
      exit(-1);
    }
  }
  close(fd);
  return EXIT_SUCCESS;
}

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVPORT 9527
#define MAXBUF 1024

int main(int argc, char *argv[])
{

  char buf[MAXBUF];
  struct sockaddr_in servaddr;
  int fd;
  int n, len;

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  servaddr.sin_port = htons(SERVPORT);

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket");
    exit(1);
  }
  int nSendBuf = 0;

  socklen_t opt_len = sizeof(int);

  nSendBuf = 3072;

  int ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &nSendBuf, sizeof(int));

  if (ret == -1)
  {
    perror("setsockopt");
  }

  getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &nSendBuf, &opt_len);

  printf("Client recvbuf len = %d\n", nSendBuf);
  if (connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
  {
    perror("connect");
    exit(1);
  }
  printf("Connection ok, sleep 30s\n");
  sleep(30); // 睡眠60秒保证  服务端缓冲区被打满
  printf("Send msg==>"
         "hello server!! do you sndbuf is fulled?\n");
  send(fd, "hello server!! do you sndbuf is fulled?",
       strlen("hello server!! do you sndbuf is fulled?"), 0);
  sleep(10);
  send(fd, "hello server!! do you sndbuf is fulled?",
       strlen("hello server!! do you sndbuf is fulled?"), 0);
  sleep(10);
  close(fd);

  return EXIT_SUCCESS;
}

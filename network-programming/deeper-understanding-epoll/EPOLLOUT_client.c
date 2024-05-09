#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVPORT 9527
#define MAXBUF 1024

int main(void)
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

  if (connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
  {
    perror("connect");
    exit(1);
  }
  printf("Connection ok, Send data after sleep 10s.\n");
  sleep(10);
  printf(">> bye bye\n");
  close(fd); // 会出发server端 EPOLLIN和EPOLLOUT
  return 0;
}

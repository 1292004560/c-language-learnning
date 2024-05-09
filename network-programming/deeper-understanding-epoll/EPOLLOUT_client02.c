
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

int main(int argc, char *argv[]) {

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
  printf(">> Send \"helloworld-1\"\n");
  send(fd, "helloworld-1", sizeof("helloworld-1") - 1, 0);
  sleep(5);
  printf(">> Send \"helloworld-2\"\n");
  send(fd, "helloworld-2", sizeof("helloworld-2") - 1, 0);
  printf("Will colse after sleep 8s.\n");
  sleep(8);
  close(fd);

  return EXIT_SUCCESS;
}

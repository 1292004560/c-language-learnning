#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define max_recv_len 1024 // buffer length

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("please enter port ! e.x. 9999.\n");

    return -1;
  }

  int listen_port = atoi(argv[1]);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server_addr;

  memset(&server_addr, 0, sizeof(struct sockaddr_in));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  server_addr.sin_port = htons(listen_port);

  if (-1 ==
      bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))) {
    printf("bind failed : %s", strerror(errno));
    return -1;
  }

  listen(sockfd, 10);

  printf("sockfd = %d ,port:%d  listening \n", sockfd, listen_port);

  struct sockaddr_in client_addr;

  socklen_t sockaddr_len = sizeof(struct sockaddr);

  int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &sockaddr_len);

  printf("connected success ! sockfd : %d, clientfd : %d\n", sockfd, clientfd);

  char buffer[max_recv_len] = {0};

  while (1) {
    memset(buffer, 0, max_recv_len);

    int recv_len = recv(clientfd, buffer, max_recv_len, 0);

    if (0 == recv_len) {
      close(clientfd);
      break;
    }

    printf("recv clientfd:%d, recv_len:%d, recv: %s\n", clientfd, recv_len,
           buffer);
    send(clientfd, buffer, max_recv_len, 0); // 发送数据
  }

  return EXIT_SUCCESS;
}

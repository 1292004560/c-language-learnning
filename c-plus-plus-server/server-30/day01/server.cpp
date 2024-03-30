#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>

int main() {

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server_addr;

  bzero(&server_addr, sizeof(server_addr));

  server_addr.sin_family = AF_INET;

  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  server_addr.sin_port = htons(8888);

  if (-1 == bind(sockfd, (sockaddr *)&server_addr, sizeof(server_addr))) {
    printf("error bind()");
    exit(1);
  }

  listen(sockfd, SOMAXCONN);

  struct sockaddr_in client_addr;

  socklen_t client_addr_len = sizeof(client_addr);

  bzero(&client_addr, sizeof(client_addr));

  int client_sockfd =
      accept(sockfd, (sockaddr *)&client_addr, &client_addr_len);

  printf("new client fd %d ! IP : %s Port : %d\n", client_sockfd,
         inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

  return 0;
}

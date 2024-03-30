#include "util.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  errif(sockfd == -1, "socket create error");

  struct sockaddr_in server_addr;
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_addr.sin_port = htons(8888);

  errif(bind(sockfd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1,
        "bind error");

  errif(listen(sockfd, SOMAXCONN) == -1, "listen error");

  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  bzero(&client_addr, sizeof(client_addr));

  int client_sockfd =
      accept(sockfd, (sockaddr *)&client_addr, &client_addr_len);

  errif(client_sockfd == -1, "accept error");

  printf("new client fd %d ! IP : %s Port : %d\n", client_sockfd,
         inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

  while (true) {
    char buffer[1024];

    bzero(&buffer, sizeof(buffer));

    ssize_t read_bytes = read(client_sockfd, buffer, sizeof(buffer));

    if (read_bytes > 0) {
      printf("meaasge from client fd %d : %s\n", client_sockfd, buffer);

      write(client_sockfd, buffer, sizeof(buffer));

    } else if (read_bytes == 0) {
      printf("client fd %d disconnected \n", client_sockfd);
      close(client_sockfd);
      // lllll break;
    } else if (read_bytes == -1) {
      close(client_sockfd);
      errif(true, "read error");
    }
  }

  close(sockfd);

  return 0;
}

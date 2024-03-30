#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server_addr;

  bzero(&server_addr, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  server_addr.sin_port = htons(8888);

  if (-1 == connect(sockfd, (sockaddr *)&server_addr, sizeof(server_addr))) {

    printf("error connect()");
    exit(1);
  }

  return 0;
}

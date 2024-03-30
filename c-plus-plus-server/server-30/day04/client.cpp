#include "util.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  errif(sockfd == -1, "socket create error");

  struct sockaddr_in server_address;
  bzero(&server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_address.sin_port = htons(8888);

  errif(connect(sockfd, (sockaddr *)&server_address, sizeof(server_address)) ==
            -1,
        "connect error");
  while (true) {
    char buffer[BUFFER_SIZE];
    bzero(&buffer, sizeof(buffer));
    scanf("%s", buffer);
    ssize_t write_bytes = write(sockfd, buffer, sizeof(buffer));

    if (write_bytes == -1) {
      printf("socket already disconnected, can't write any more!\n");
      break;
    }

    bzero(&buffer, sizeof(buffer));
    ssize_t read_bytes = read(sockfd, buffer, sizeof(buffer));

    if (read_bytes > 0) {
      printf("message from server : %s \n", buffer);
    } else if (read_bytes == 0) {
      printf("server socket disconnected!\n");
    } else if (read_bytes == -1) {
      close(sockfd);
      errif(true, "socket read error");
    }
  }
  close(sockfd);
  return 0;
}

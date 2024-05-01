#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define UNIXSTR_PATH "/tmp/unix.str"

#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {

  int sockfd;

  sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

  struct sockaddr_un server_address;

  bzero(&server_address, sizeof(server_address));
  server_address.sun_family = AF_LOCAL;
  strcpy(server_address.sun_path, UNIXSTR_PATH);

  connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));

  char buf[BUFFER_SIZE];

  while (1) {
    bzero(buf, sizeof(buf));
    printf(">> ");

    if (fgets(buf, BUFFER_SIZE, stdin) == NULL) {
      break;
    }

    write(sockfd, buf, strlen(buf));
  }

  close(sockfd);

  return EXIT_SUCCESS;
}

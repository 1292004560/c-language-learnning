#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define UNIXSTR_PATH "/tmp/unix.str"

#define LISTENQ 5
#define BUFFER_SZIE 256

int main(int argc, char *argv[]) {

  int listenfd, connfd;

  socklen_t len;

  struct sockaddr_un server_address, client_address;

  if (-1 == (listenfd = socket(AF_LOCAL, SOCK_STREAM, 0))) {
    perror("socket error ");
    exit(EXIT_FAILURE);
  }

  unlink(UNIXSTR_PATH);

  bzero(&server_address, sizeof(server_address));
  server_address.sun_family = AF_LOCAL;
  strcpy(server_address.sun_path, UNIXSTR_PATH);

  if (-1 == bind(listenfd, (struct sockaddr *)&server_address,
                 sizeof(server_address))) {
    perror("bind error");

    exit(EXIT_FAILURE);
  }

  listen(listenfd, LISTENQ);

  len = sizeof(client_address);

  if (-1 ==
      (connfd = accept(listenfd, (struct sockaddr *)&client_address, &len))) {
    perror("accept error");
    exit(EXIT_FAILURE);
  }

  char buf[BUFFER_SZIE];

  while (1) {

    bzero(buf, sizeof(buf));

    if (read(connfd, buf, BUFFER_SZIE) == 0)
      break;

    printf("receive : %s", buf);
  }

  close(listenfd);
  close(connfd);
  unlink(UNIXSTR_PATH);

  return EXIT_SUCCESS;
}

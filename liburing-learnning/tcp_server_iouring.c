#include <liburing.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define ENTRIES_LENGTH 4096
#define MAX_CONNECTIONS 1024
#define BUFFER_LENGTH 1024

char buffer_table[MAX_CONNECTIONS][BUFFER_LENGTH] = {0};

enum {

  READ,
  WRITE,
  ACCEPT,

};

struct connection_information {

  int connectionfd;
  int type;
};

void set_read_event(struct io_uring *ring, int fd, void *buffer, size_t len,
                    int flags) {

  struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
  io_uring_prep_recv(sqe, fd, buffer, len, flags);
  struct connection_information info = {
      .connectionfd = fd,
      .type = READ,
  };

  memcpy(&sqe->user_data, &info, sizeof(struct connection_information));
}

void set_write_event(struct io_uring *ring, int fd, const void *buffer,
                     size_t len, int flags) {
  struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
  io_uring_prep_send(sqe, fd, buffer, len, flags);

  struct connection_information info = {
      .connectionfd = fd,
      .type = WRITE,
  };
  memcpy(&sqe->user_data, &info, sizeof(struct connection_information));
}

void set_accept_event(struct io_uring *ring, int fd,
                      struct sockaddr *client_address,
                      socklen_t *client_address_length, unsigned flags) {

  struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
  io_uring_prep_accept(sqe, fd, client_address, client_address_length, flags);

  struct connection_information info = {
      .connectionfd = fd,
      .type = ACCEPT,
  };
  memcpy(&sqe->user_data, &info, sizeof(struct connection_information));
}
int main(int argc, char *argv[]) {

  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd == -1)
    return -1;
  struct sockaddr_in server_address, client_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(9999);

  if (-1 == bind(listenfd, (struct sockaddr *)&server_address,
                 sizeof(server_address))) {
    return -2;
  }

  listen(listenfd, 10);
  struct io_uring_params params;

  memset(&params, 0, sizeof(params));

  struct io_uring ring;
  memset(&ring, 0, sizeof(ring));

  /*初始化params 和 ring*/
  io_uring_queue_init_params(ENTRIES_LENGTH, &ring, &params);

  socklen_t client_address_length = sizeof(client_address);

  set_accept_event(&ring, listenfd, (struct sockaddr *)&client_address,
                   &client_address_length, 0);

  while (1) {
    struct io_uring_cqe *cqe;
    io_uring_submit(&ring);

    int ret = io_uring_wait_cqe(&ring, &cqe);
    struct io_uring_cqe *cqes[10];

    int cqe_count = io_uring_peek_batch_cqe(&ring, cqes, 10);

    unsigned count = 0;

    for (int i = 0; i < cqe_count; i++) {
      cqe = cqes[i];
      count++;

      struct connection_information info;
      memcpy(&info, &cqe->user_data, sizeof(info));

      if (info.type == ACCEPT) {

        int connectionfd = cqe->res;
        char *buffer = buffer_table[connectionfd];
        set_read_event(&ring, connectionfd, buffer, 1024, 0);
        set_accept_event(&ring, listenfd, (struct sockaddr *)&client_address,
                         &client_address_length, 0);

      } else if (info.type == READ) {
        int bytes_read = cqe->res;
        if (bytes_read == 0) {
          close(info.connectionfd);
        } else if (bytes_read < 0) {
          close(info.connectionfd);
          printf("client %d disconnected\n", info.connectionfd);
        } else {
          char *buffer = buffer_table[info.connectionfd];
          set_write_event(&ring, info.connectionfd, buffer, bytes_read, 0);
        }
      } else if (info.type == WRITE) {
        char *buffer = buffer_table[info.connectionfd];
        set_write_event(&ring, info.connectionfd, buffer, 1024, 0);
      }
    }
    io_uring_cq_advance(&ring, count);
  }

  return EXIT_SUCCESS;
}

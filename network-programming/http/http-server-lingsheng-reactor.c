#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_LENGTH 4096
#define MAX_EPOLL_EVENTS 1024
#define SERVER_PORT 8080
#define PORT_COUNT 100
#define HTTP_METHOD_GET 0
#define HTTP_METHOD_POST 1
#define HTTP_WEBSERVER_HTML_ROOT "html"

typedef int (*NCALLBACK)(int, int, void *);

struct ntyevent {
  int fd;
  int events;
  void *arg;

  NCALLBACK callback;

  int status;
  char buffer[BUFFER_LENGTH];
  int length;

  // http param
  int method; //
  char resource[BUFFER_LENGTH];
  int ret_code;
  char Content_Type[512];
};
struct eventblock {
  struct eventblock *next;
  struct ntyevent *events;
};

struct ntyreactor {
  int epfd;
  int blkcnt;
  struct eventblock *evblk;
};

int recv_cb(int fd, int events, void *arg);

int send_cb(int fd, int events, void *arg);

struct ntyevent *ntyreactor_find_event_idx(struct ntyreactor *reactor,
                                           int sockfd);

void nty_event_set(struct ntyevent *ev, int fd, NCALLBACK callback, void *arg) {
  ev->fd = fd;
  ev->callback = callback;
  ev->events = 0;
  ev->arg = arg;
}

int nty_event_add(int epfd, int events, struct ntyevent *ev) {

  struct epoll_event ep_ev = {0, {0}};
  ep_ev.data.ptr = ev;
  ep_ev.events = ev->events = events;
  int op;

  if (ev->status == 1) {
    op = EPOLL_CTL_MOD;
  } else {
    op = EPOLL_CTL_ADD;
    ev->status = 1;
  }
  if (epoll_ctl(epfd, op, ev->fd, &ep_ev) < 0) {
    printf("event add failed [fd=%d], events[%d]\n", ev->fd, events);
    return -1;
  }
  return 0;
}

#include "epoll.h"
#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void epoll_run(int lfd) {
  int efd = epoll_create(EVENT_MAX);

  if (efd == -1) {
    perror("epoll_create err");
    exit(1);
  }

  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = lfd;

  int ret = epoll_ctl(efd, EPOLL_CTL_ADD, lfd, &ev);

  if (ret == -1) {
    perror("epoll_ctl err");
    exit(1);
  }
}
void do_read(int cfd, int efd) {
  char line[1024];
  int len = get_line(cfd, line, sizeof(line));
  printf("head:%s", line);
  while (len) {
    char buf[1024];
    len = get_line(cfd, buf, sizeof buf);
    printf("%s", buf);
  }
  if (strncasecmp("get", line, 3) == 0) {
    http_get_request(line, cfd);
    disconnect(cfd, efd);
  }
}

void http_get_request(const char *line, int cfd) {
  char method[12] = {0};
  char res[1024] = {0};

  sscanf(line, "%[^ ] %[^ ]", method, res);
  // 转码 将不能识别的中文乱码 - > 中文
  // 解码 %23 %34 %5f

  char destr[1024];
  decode_str(destr, res);
  // 处理path  /xx
  // 去掉path中的/
  char *file = destr + 1;
  char fbuf[1024];
  if (strcmp(res, "/") == 0) {
    file = "./";
    // getcwd(fbuf, sizeof(fbuf));
    // printf("root:[%s]\n", fbuf);
    //    file = fbuf;
  }

  struct stat st;

  printf("file :[%s]\n", file);
  int ret = stat(file, &st);

  if (ret == -1) {
    // todo 404
    perror("stat error");
    exit(1);
  }

  if (S_ISDIR(st.st_mode)) {
    // dir
    // 发送头信息
    send_respond_head(cfd, 200, "OK", get_file_type(".html"), 100000);
    send_dir(cfd, file);
  } else if (S_ISREG(st.st_mode)) {
    // reg file
    send_respond_head(cfd, 200, "OK", get_file_type(file), st.st_size);
    send_file(cfd, file);
  }
}

void send_dir(int cfd, const char *path) {
  char file[1024] = {0};
  char buf[4096] = {0};

  sprintf(buf, "<html><head><title>目录名: %s</title></head>", path);
  sprintf(buf + strlen(buf), "<body><h1>当前目录: %s</h1><table>", path);

#if 1
  DIR *dir = opendir(path);
  if (dir == NULL) {
    perror("opendir error");
    exit(1);
  }
  // 读目录
  char enstr[1024] = {0};
  struct dirent *ptr = NULL;
  while ((ptr = readdir(dir)) != NULL) {
    char *name = ptr->d_name;
    // 拼接文件的完整路径
    sprintf(file, "%s/%s", path, name);

    encode_str(enstr, sizeof(enstr), name);
    struct stat st;
    int ret = stat(file, &st);
    if (ret == -1) {
      perror("stat err");
      exit(1);
    }
    // 如果是文件
    if (S_ISREG(st.st_mode)) {
      sprintf(buf + strlen(buf),
              "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>", enstr,
              name, (long)st.st_size);
    }
    // 如果是目录
    else if (S_ISDIR(st.st_mode)) {
      sprintf(buf + strlen(buf),
              "<tr><td><a href=\"%s/\">%s/</a></td><td>%ld</td></tr>", enstr,
              name, (long)st.st_size);
    }
    send(cfd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));
  }
  sprintf(buf + strlen(buf), "</table></body></html>");
  send(cfd, buf, strlen(buf), 0);
#endif
}

void disconnect(int cfd, int epfd) {
  int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
  if (ret == -1) {
    perror("epoll_ctl del cfd error");
    exit(1);
  }
  close(cfd);
}

void send_file(int cfd, const char *path) {
  int fd = open(path, O_RDONLY);
  if (fd == -1) {
    perror("read err");
    exit(1);
  }
  close(fd);
}

void send_respond_head(int cfd, int no, const char *desp, const char *type,
                       long len) {
  char buf[1024] = {0};
  // 状态行
  sprintf(buf, "http/1.1 %d %s\r\n", no, desp);
  send(cfd, buf, strlen(buf), 0);
  // 消息报头
  memset(buf, 0, sizeof buf);
  sprintf(buf, "Content-Type:%s\r\n", type);
  sprintf(buf + strlen(buf), "Content-Length:%ld\r\n", len);
  send(cfd, buf, strlen(buf), 0);
  // 空行
  send(cfd, "\r\n", 2, 0);
}

void do_accept(int lfd, int efd) {
  struct sockaddr_in client;

  socklen_t len = sizeof(client);
  int cfd = accept(lfd, (struct sockaddr *)&client, &len);
  if (cfd == -1) {
    perror("accept err");
    exit(1);
  }
  // set nonblock socket
  int flag = fcntl(cfd, F_GETFL);
  flag |= O_NONBLOCK;
  fcntl(cfd, F_SETFL, flag);

  struct epoll_event ev;
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = cfd;
  int ret = epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &ev);
  if (ret == -1) {
    perror("epoll_ctl err");
    exit(1);
  }
}

// 解析http请求消息的每一行内容
int get_line(int sock, char *buf, int size) {
  int i = 0;
  char c = '\0';
  int n;
  while ((i < size - 1) && (c != '\n')) {
    n = recv(sock, &c, 1, 0);
    if (n > 0) {
      if (c == '\r') {
        n = recv(sock, &c, 1, MSG_PEEK);
        if ((n > 0) && (c == '\n')) {
          recv(sock, &c, 1, 0);
        } else {
          c = '\n';
        }
      }
      buf[i] = c;
      i++;
    } else {
      c = '\n';
    }
  }
  buf[i] = '\0';

  return i;
}

// 通过文件名获取文件的类型
const char *get_file_type(const char *name) {
  char *dot;

  // 自右向左查找‘.’字符, 如不存在返回NULL
  dot = strrchr(name, '.');
  if (dot == NULL)
    return "text/plain; charset=utf-8";
  if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
    return "text/html; charset=utf-8";
  if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
    return "image/jpeg";
  if (strcmp(dot, ".gif") == 0)
    return "image/gif";
  if (strcmp(dot, ".png") == 0)
    return "image/png";
  if (strcmp(dot, ".css") == 0)
    return "text/css";
  if (strcmp(dot, ".au") == 0)
    return "audio/basic";
  if (strcmp(dot, ".wav") == 0)
    return "audio/wav";
  if (strcmp(dot, ".avi") == 0)
    return "video/x-msvideo";
  if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
    return "video/quicktime";
  if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
    return "video/mpeg";
  if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
    return "model/vrml";
  if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
    return "audio/midi";
  if (strcmp(dot, ".mp3") == 0)
    return "audio/mpeg";
  if (strcmp(dot, ".ogg") == 0)
    return "application/ogg";
  if (strcmp(dot, ".pac") == 0)
    return "application/x-ns-proxy-autoconfig";

  return "text/plain; charset=utf-8";
}

// 16进制数转化为10进制
int hexit(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;

  return 0;
}

/*
 *  这里的内容是处理%20之类的东西！是"解码"过程。
 *  %20 URL编码中的‘ ’(space)
 *  %21 '!' %22 '"' %23 '#' %24 '$'
 *  %25 '%' %26 '&' %27 ''' %28 '('......
 *  相关知识html中的‘ ’(space)是&nbsp
 */
void encode_str(char *to, int tosize, const char *from) {
  int tolen;

  for (tolen = 0; *from != '\0' && tolen + 4 < tosize; ++from) {
    if (isalnum(*from) || strchr("/_.-~", *from) != (char *)0) {
      *to = *from;
      ++to;
      ++tolen;
    } else {
      sprintf(to, "%%%02x", (int)*from & 0xff);
      to += 3;
      tolen += 3;
    }
  }
  *to = '\0';
}

void decode_str(char *to, char *from) {
  for (; *from != '\0'; ++to, ++from) {
    if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2])) {

      *to = hexit(from[1]) * 16 + hexit(from[2]);

      from += 2;
    } else {
      *to = *from;
    }
  }
  *to = '\0';
}

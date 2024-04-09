#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <arpa/inet.h>
#include <cassert>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "./threadpool/threadpool.h"
#include "/http/http_conn.h"

const int MAX_FD = 65535;           // 最大文件描述符
const int MAX_EVENT_NUMBER = 10000; // 最大事件数
const int TIMESLOT = 5;             // 最小超时单位

#endif

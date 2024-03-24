/*
 * zv开头的变量是zvnet异步网络库（epoll）。
 * kv开头的变量是kv存储协议解析。
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "kvstore.h"

/*-------------------------------------------------------*/
/*-----------------------异步网路库-----------------------*/
/*-------------------------------------------------------*/
/*-----------------------函数声明-------------------------*/
#define max_buffer_len      1024    // 读写buffer长度
#define epoll_events_size   1024    // epoll就绪集合大小
#define connblock_size      1024    // 单个连接块存储的连接数量
#define listen_port_count   1       // 监听端口总数


// 有如下参数列表和返回之类型的函数都称为 CALLBACK
// 回调函数，方便在特定epoll事件发生时执行相应的操作

typedef int (*ZV_CALLBACK)(int fd, int events, void * arg);

// 回调函数: 建立连接

int accept_cb(int fd, int event, void * arg);

// 回调函数：发送数据
int send_cb(int clientfd, int event, void* arg);


// 单个连接
typedef struct zv_connect_s
{
    // 本连接的客户端fd
    int fd;
    // 本连接的读写buffer
    char rbuffer[max_buffer_len];
    size_t rcount;  // 读buffer的实际大小
    char wbuffer[max_buffer_len];
    size_t wcount;  // 写buffer的实际大小
    size_t next_len;  // 下一次读数据长度（读取多个包会用到）
    // 本连接的回调函数--accept()/recv()/send()
    ZV_CALLBACK cb;
}zv_connect;


typedef struct zv_connblock_s
{

  struct zv_connect_s *block; // 指向的当前块，注意大小为 connblock_size
  struct zv_connblock_s *next;// 指向的下一个连接块的头
}zv_connblock;







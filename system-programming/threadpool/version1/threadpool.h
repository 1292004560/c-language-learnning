#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <malloc.h>
#include <memory.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// 头插法
#define LL_ADD(item, list)                                                     \
  do {                                                                         \
    item->prev = NULL;                                                         \
    item->next = list;                                                         \
    if (list != NULL) {                                                        \
      list->prev = item;                                                       \
    }                                                                          \
    list = item;                                                               \
  } while (0)

#define LL_REMOVE(item, list)                                                  \
  do {                                                                         \
    if (item->prev != NULL) {                                                  \
      item->prev->next = item->next;                                           \
    }                                                                          \
    if (item->next != NULL) {                                                  \
      item->next->prev = item->prev;                                           \
    }                                                                          \
    if (list == item) {                                                        \
      list = item->next;                                                       \
    }                                                                          \
    item->prev = item->next = NULL;                                            \
  } while (0)

// 执行队列
typedef struct NWORKER {
  pthread_t id;
  int termination;
  struct NTHREADPOOL *thread_pool;

  struct NWORKER *prev;
  struct NWORKER *next;
} worker_t;

// 任务队列
typedef struct NTASK {
  void (*task_func)(void *arg);

  void *user_data;

  struct NTASK *prev;
  struct NTASK *next;
} task_t;

// 池管理组件
typedef struct NTHREADPOOL {
  worker_t *workers;
  task_t *tasks;

  pthread_cond_t cond;
  pthread_mutex_t mutex;
} thread_pool_t;

task_t *get_task(worker_t *worker);
void *thread_callback(void *arg);
int thread_pool_create(thread_pool_t *thread_pool, int thread_num);
int thread_pool_push_task(thread_pool_t *thread_pool, task_t *task);
int thread_destroy(thread_pool_t *thread_pool);

#endif

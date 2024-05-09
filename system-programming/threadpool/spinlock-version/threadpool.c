#include "threadpool.h"
#include "spinlock.h"
#include <pthread.h>
#include <stdlib.h>

typedef struct spinlock spinlock_t;

typedef struct task_s {

  void *next;
  handler_t handler;
  void *arg;
} task_t;

typedef struct task_queue_s {

  void *head;
  void **tail;
  int block;
  spinlock_t lock;

  pthread_mutex_t mutex;

  pthread_cond_t cond;

} task_queue_t;

struct thread_pool_s {

  task_queue_t *task_queue;
  atomic_int quit;
  int thread_count;
  pthread_t *threads;
};

static task_queue_t *__task_queue_create() {

  task_queue_t *task_queue = (task_queue_t *)malloc(sizeof(*task_queue));
  if (!task_queue) {
    return NULL;
  }
  int ret;
  ret = pthread_mutex_init(&task_queue->mutex, NULL);
  if (ret == 0) {
    ret = pthread_cond_init(&task_queue->cond, NULL);

    if (ret == 0) {
      spinlock_init(&task_queue->lock);
      task_queue->head = NULL;
      task_queue->tail = &task_queue->head;
      task_queue->block = 1; // 默认为阻塞队列
      return task_queue;
    }

    pthread_cond_destroy(&task_queue->cond);
  }
  pthread_mutex_destroy(&task_queue->mutex);
  return NULL;
}

static void __task_queue_destroy(task_queue_t *task_queue) {
  if (!task_queue) {
    return;
  }
}

static void __nonblock(task_queue_t *task_queue) {

  pthread_mutex_lock(&task_queue->mutex);
  task_queue->block = 0;
  pthread_mutex_unlock(&task_queue->mutex);
  pthread_cond_broadcast(&task_queue->cond);
}

static inline void __add_task(task_queue_t *task_queue, void *task) {

  void **link = (void **)task; // 转成二级指针的意思是 让link 指向next
  *link = NULL;                // 这两句话 就是 task->next = NULL;
  spinlock_lock(&task_queue->lock);
  *task_queue->tail = link;
  task_queue->tail = link;
  spinlock_unlock(&task_queue->lock);
  pthread_cond_signal(&task_queue->cond);
}

static inline void *__pop_task(task_queue_t *task_queue) {

  spinlock_lock(&task_queue->lock);
  if (task_queue->head == NULL) { // 队列是否为空
    spinlock_unlock(&task_queue->lock);
    return NULL;
  }
  task_t *task;
  task = task_queue->head;
  task_queue->head = task->next;
  if (task_queue->head == NULL) {
    task_queue->tail = &task_queue->head;
  }
  spinlock_unlock(&task_queue->lock);
  return task;
}

// 如果没有任务 让当前线程池休眠
static inline void *__get_task(task_queue_t *task_queue) {

  task_t *task;
  while ((task = __pop_task(task_queue)) == NULL) {
    pthread_mutex_lock(&task_queue->mutex);
    if (task_queue->block == 0) {
      pthread_mutex_unlock(&task_queue->mutex);
      break;
    }
    pthread_cond_wait(&task_queue->cond, &task_queue->mutex); // 休眠位置
    pthread_mutex_unlock(&task_queue->mutex);
  }

  return task;
}

static void __thread_pool_destroy(task_queue_t *task_queue) {
  task_t *task;

  while (task == __pop_task(task_queue)) {
    free(task); // 任务生命周期由线程池管理
  }
  pthread_cond_destroy(&task_queue->cond);
  pthread_mutex_destroy(&task_queue->mutex);
  spinlock_destroy(&task_queue->lock);
  free(task_queue);
}

static void *__thread_pool_worker(void *arg) {

  thread_pool_t *pool = (thread_pool_t *)arg;
  task_t *task;

  void *context;

  while (atomic_load(&pool->quit) == 0) {
    task = (task_t *)__get_task(pool->task_queue);
    if (!task)
      break;
    handler_t handler = task->handler;
    context = task->arg;
    free(task);
    handler(context);
  }
  return NULL;
}

static void __threads_terminate(thread_pool_t *pool) {

  atomic_store(&pool->quit, 1);
  __nonblock(pool->task_queue);
  int i;
  for (i = 0; i < pool->thread_count; i++) {
    pthread_join(pool->threads[i], NULL);
  }
}

static int __threads_create(thread_pool_t *pool, size_t thread_count) {

  pthread_attr_t attr;
  int ret;

  ret = pthread_attr_init(&attr);
  if (ret == 0) {
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
    if (pool->threads) {
      int i;
      for (i = 0; i < thread_count; i++) {
        if (pthread_create(&pool->threads[i], &attr, __thread_pool_worker,
                           pool)) {
          break;
        }
      }
      pool->thread_count = i;
      pthread_attr_destroy(&attr);
      if (i == thread_count) {
        return 0; // 正确返回
      }
      __threads_terminate(pool);
    }
    ret = -1;
  }
  return ret;
}

void thread_pool_terminate(thread_pool_t *pool) {

  atomic_store(&pool->quit, 1);
  __nonblock(pool->task_queue);
}

thread_pool_t *thread_pool_create(int thread_count) {

  thread_pool_t *pool;
  pool = (thread_pool_t *)malloc(sizeof(*pool));
  if (!pool)
    return NULL;

  task_queue_t *task_queue = __task_queue_create();
  if (task_queue) {

    pool->task_queue = task_queue;
    atomic_init(&pool->quit, 0);
    if (__threads_create(pool, thread_count) == 0) {
      return pool;
    }
    __task_queue_destroy(pool->task_queue);
  }
  free(pool);
  return NULL;
}

int thread_pool_post(thread_pool_t *pool, handler_t handler, void *arg) {

  if (atomic_load(&pool->quit) == 1) {
    return -1;
  }
  task_t *task = malloc(sizeof(task_t));
  if (!task)
    return -1;
  task->handler = handler;
  task->arg = arg;
  __add_task(pool->task_queue, task);
  return 0;
}

void thread_pool_waitdone(thread_pool_t *pool) {
  int i;
  for (i = 0; i < pool->thread_count; i++) {
    pthread_join(pool->threads[i], NULL);
  }
  __task_queue_destroy(pool->task_queue);
  free(pool->threads);
  free(pool);
}

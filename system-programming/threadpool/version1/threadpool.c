#include "threadpool.h"

task_t *get_task(worker_t *worker) {
  while (1) {
    pthread_mutex_lock(&worker->thread_pool->mutex);
    while (worker->thread_pool->workers == NULL) {
      if (worker->termination)
        break;
      pthread_cond_wait(&worker->thread_pool->cond,
                        &worker->thread_pool->mutex);
    }
    if (worker->termination) {
      pthread_mutex_unlock(&worker->thread_pool->mutex);
      return NULL;
    }
    task_t *task = worker->thread_pool->tasks;
    if (task) {
      LL_REMOVE(task, worker->thread_pool->tasks);
    }
    pthread_mutex_unlock(&worker->thread_pool->mutex);
    if (task != NULL) {
      return task;
    }
  }
};

void *thread_callback(void *arg) {
  worker_t *worker = (worker_t *)arg;
  while (1) {
    task_t *task = get_task(worker);
    if (task == NULL) {
      free(worker);
      pthread_exit("thread termination\n");
    }
    task->task_func(task);
  }
}

// return access create thread num;
int thread_pool_create(thread_pool_t *thread_pool, int thread_num) {
  if (thread_num < 1)
    thread_num = 1;
  memset(thread_pool, 0, sizeof(thread_pool_t));

  // init cond
  pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
  memcpy(&thread_pool->cond, &blank_cond, sizeof(pthread_cond_t));
  // init mutex
  pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
  memcpy(&thread_pool->mutex, &blank_mutex, sizeof(pthread_mutex_t));

  // one thread one worker
  int idx = 0;
  for (idx = 0; idx < thread_num; idx++) {
    worker_t *worker = malloc(sizeof(worker_t));
    if (worker == NULL) {
      perror("worker malloc err\n");
      return idx;
    }
    memset(worker, 0, sizeof(worker_t));
    worker->thread_pool = thread_pool;

    int ret = pthread_create(&worker->id, NULL, thread_callback, worker);
    if (ret) {
      perror("pthread_create err\n");
      free(worker);
      return idx;
    }
    LL_ADD(worker, thread_pool->workers);
  }
  return idx;
}

int thread_pool_push_task(thread_pool_t *thread_pool, task_t *task) {
  pthread_mutex_lock(&thread_pool->mutex);
  LL_ADD(task, thread_pool->tasks);
  pthread_cond_signal(&thread_pool->cond);
  pthread_mutex_unlock(&thread_pool->mutex);
}

int thread_destroy(thread_pool_t *thread_pool) {
  worker_t *worker = NULL;
  for (worker = thread_pool->workers; worker != NULL; worker = worker->next) {
    worker->termination = 1;
  }
  pthread_mutex_lock(&thread_pool->mutex);
  pthread_cond_broadcast(&thread_pool->cond);
  pthread_mutex_unlock(&thread_pool->mutex);
}

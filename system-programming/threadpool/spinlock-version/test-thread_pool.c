#include "threadpool.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t lock;

int done;

void do_task(void *arg) {
  thread_pool_t *pool = (thread_pool_t *)arg;
  pthread_mutex_lock(&lock);
  done++;
  printf("doing %d task\n", done);

  pthread_mutex_unlock(&lock);
  if (done >= 1000) {
    thread_pool_terminate(pool);
  }
}

int main(int argc, char *argv[]) {

  int threads = 8;
  pthread_mutex_init(&lock, NULL);
  thread_pool_t *pool = thread_pool_create(threads);
  if (pool == NULL) {
    perror("thread pool create error!\n");
    exit(-1);
  }

  while (thread_pool_post(pool, &do_task, pool) == 0) {
  }

  thread_pool_waitdone(pool);
  pthread_mutex_destroy(&lock);
  return EXIT_SUCCESS;
}

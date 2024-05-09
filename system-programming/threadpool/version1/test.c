#include "threadpool.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define THREAD_COUNT 10
#define TASK_COUNT 1000

void counter(task_t *task) {
  int idx = *(int *)task->user_data;
  printf("idx:%d  pthread_id:%llu\n", idx, (unsigned long long)pthread_self());

  free(task->user_data);
  free(task);
}

int main() {
  thread_pool_t thread_pool = {0};
  int ret = thread_pool_create(&thread_pool, THREAD_COUNT);
  if (ret != THREAD_COUNT) {
    thread_destroy(&thread_pool);
  }
  int i = 0;
  for (i = 0; i < TASK_COUNT; i++) {
    // create task
    task_t *task = (task_t *)malloc(sizeof(task_t));
    if (task == NULL) {
      perror("task malloc err\n");
      exit(1);
    }
    task->task_func = counter;
    task->user_data = malloc(sizeof(int));
    *(int *)task->user_data = i;
    // push task
    thread_pool_push_task(&thread_pool, task);
  }
  getchar();
  thread_destroy(&thread_pool);
}

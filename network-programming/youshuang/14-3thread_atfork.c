#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

pthread_mutex_t mutex;

/*子线程运行的函数。它首先获得互斥锁mutex，然后暂停5
 * s，再释放该互斥锁*/
void *another(void *arg) {
  printf("in child thread, lock the mutex\n");
  pthread_mutex_lock(&mutex);
  sleep(5);
  pthread_mutex_unlock(&mutex);
}

void prepare() { pthread_mutex_lock(&mutex); }

void infork() { pthread_mutex_unlock(&mutex); }

int main(int argc, char *argv[]) {
  pthread_mutex_init(&mutex, NULL);

  pthread_t tid;
  pthread_create(&tid, NULL, another, NULL);
  /*父进程中的主线程暂停1
  s，以确保在执行fork操作之前，子线程已经开始运行并获
  得了互斥变量mutex*/
  sleep(1);

  int pid = fork();

  if (pid < 0) {
    pthread_join(tid, NULL);
    pthread_mutex_destroy(&mutex);
    return 1;
  } else if (pid == 0) {
    /*子进程从父进程继承了互斥锁mutex的状态，该互斥锁处于锁住的状态，这是由父进程中的子线程执行pthread_mutex_lock引起的
     * ，因此，下面这句加锁操作会一直阻塞，尽管从逻辑上来说它是不应该阻塞的*/
    printf("I am in the child, want to get the lock\n");
    pthread_mutex_lock(&mutex);
    printf("I can not run to here, oop...\n");
    pthread_mutex_unlock(&mutex);
    exit(0);
  } else {
    pthread_mutex_unlock(&mutex);
    wait(NULL);
  }

  pthread_join(tid, NULL);
  pthread_mutex_destroy(&mutex);
  return EXIT_SUCCESS;
}

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {

  sem_t *lock;
  int num;
} STRUCT;

void test(void *obj) {

  STRUCT *point = (STRUCT *)obj;

  sem_t *semlock = point->lock;
  sem_wait(semlock);

  FILE *f = fopen("text.txt", "a");

  if (f == NULL)
    printf("fopen is wrong\n");

  printf("sem_wait %d\n", point->num);

  int j = 0;

  for (j = 0; j < 30; j++) {
    fprintf(f, "%c 111111111111\n", 'a' + point->num);
  }
  fclose(f);
  sem_post(semlock);
}

int main(int argc, char *argv[]) {

  pthread_t pid[20];

  int ret, i = 0;

  STRUCT obj[13];

  sem_t semlock;
  if (sem_init(&semlock, 0, 1) != 0)
    printf("sem_init is wrong\n");

  for (i = 0; i < 10; i++) {
    obj[i].num = i;
    obj[i].lock = &semlock;
    ret = pthread_create(&pid[i], NULL, (void *)test, &obj[i]);

    if (ret != 0) {
      printf("create thread wrong %d !\n", i);
      return 0;
    }
  }

  for (i = 0; i < 10; i++) {
    pthread_join(pid[i], NULL);
  }

  return EXIT_SUCCESS;
}

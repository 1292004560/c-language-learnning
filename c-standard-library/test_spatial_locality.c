#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define M 10000
#define N 10000
int arr[M][N];

void init() {
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      arr[i][j] = 1;
    }
  }
}

double sum_array_rows() {
  clock_t start = clock();

  int i, j, sum;

  for (i = 0; i < M; i++) {
    for (j = 0; j < N; j++) {
      sum += arr[i][j];
    }
  }

  clock_t end = clock();

  return (double)(end - start) / CLOCKS_PER_SEC;
}

double sum_array_clos() {
  clock_t start = clock();

  int i, j, sum;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; j++) {
      sum += arr[j][i];
    }
  }

  clock_t end = clock();

  return (double)(end - start) / CLOCKS_PER_SEC;
}
int main(int argc, char *argv[]) {

  init();
  printf("sum_array_rows running %f \n", sum_array_rows());

  init();
  printf("sum_array_clos running %f \n", sum_array_clos());
  return EXIT_SUCCESS;
}

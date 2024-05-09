#include <stdio.h>
#include <stdlib.h>

#define MAXBUFFER 1024

int main(int argc, char *argv[]) {

  char buffer[MAXBUFFER] = {1};

  for (int i = 0; i < MAXBUFFER; i++) {
    printf("%d \n", buffer[i]);
  }

  return EXIT_SUCCESS;
}

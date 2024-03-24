#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{


  int n;

  char str[80];

  printf("enter a number: ");

  scanf("%d", &n);

  printf("enter a string : ");
  gets(str);

  printf("you typed %d and %s \n", n, str);



  return EXIT_SUCCESS;
}

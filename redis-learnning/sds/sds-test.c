#include <stdio.h>
#include <math.h>
#include "sds.h"
int main()
{
  sds  s = sdsnew("sssss");

  printf("%s", s);
  return 0;
}

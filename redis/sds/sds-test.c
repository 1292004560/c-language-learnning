#include <stdio.h>
#include <math.h>
#include "sds.h"

//double sample_stddev(int count, ...){
//  double  sum = 0;
//  va_list  args1;
//  va_start(args1, count);
//  va_list args2;
//  va_copy(args2, args1);
//
//  for (int i = 0; i < count; ++i) {
//    double num = va_arg(args1, double );
//    sum += num;
//  }
//  va_end(args1);
//
//  double mean = sum / count;
//
//  double sum_sq_diff = 0;
//  for (int i = 0; i < count; ++i) {
//    double num = va_arg(args2, double );
//    sum_sq_diff += (num - mean) * (num - mean);
//  }
//  va_end(args2);
//  return sqrt(sum_sq_diff/count);
//}
//
//
//char buffer[80];
//int vspfunc(char * format, ...)
//{
//  va_list apstr;
//  int ret;
//
//  va_start(apstr, format);
//  ret = vsprintf(buffer, format, apstr);
//  va_end(apstr);
//  return (ret);
//}


//int main()
//{
//  printf("%f\n", sample_stddev(4, 25.0, 27.3, 26.9, 25.7));
//  char * s;
//  printf("%d\n", s[-1]);
//
//  int i = 5;
//  float f = 27.0;
//  char str[50] = "runoob.com";
//
//  vspfunc("%d %f %s", i, f, str);
//  printf("%s\n", buffer);
//  return 0;
//}


int main()
{
  sds  s = sdsnew("sssss");
//  sdscatfmt(s, "%d", 2);

  printf("%s", s);
  return 0;
}

#include <iostream>
#include <cmath>
#include <locale>



long double operator"" _d2r(long double degree)
{ 
  return degree * 3.1415926 / 360.0; 
}

int main (int argc, char *argv[]) {
  

  
  //带千分位分隔符的二进制常量
 int a = 0b1'010'101;

 //普通字符串，其中\n 是转义字符
 const char *s1 = "one\ntwo";

 //原始字符串。*是分隔符。符号串\n 不再是转义字符，而是它们本身
 const char *s2 = R"*(one\ntwo)*";

 //utf-8 编码的字符串
 const char *s3 = u8"Unicode Characters: \u4e2d\u6587";

 std::cout << a << std::endl;
 //第一次调用将 30.0 视作弧度值
 std::cout << sin(30.0) << ',' << sin(30.0_d2r) << std::endl;


 std::cout << s1 << std::endl;
 std::cout << s2 << std::endl;
 std::cout << s3 << std::endl;

  return 0;
}

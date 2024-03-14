#include <iostream>

int main (int argc, char *argv[]) {
  

  int a = 0;
  int & ra = a;

  std::cout << "a=" << a << ',' << "ra=" <<ra << std::endl;

  ++ra;
  
  std::cout << "a=" << a << ',' << "ra=" << ra << std::endl;

  int b = 3;
  ra = b; //这不是引用绑定，而是赋值
  std::cout << "a=" << a << ',' << "b=" << b << ',' << "ra=" << ra << std::endl;
  return 0;
}

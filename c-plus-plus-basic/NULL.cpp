#include <iostream>

void func(char * str)
{
  std::cout << "char * version" << std::endl;
}

void func(int i)
{
  std::cout << "int version" << std::endl;
}

int main (int argc, char *argv[]) {
//  func(NULL);

  func(nullptr);
  return 0;
}

#include <functional>
#include <iostream>
typedef int (*func)();

int print1()
{

  printf("hello world");

  return 0;
}

int print2()
{

  printf("hello printf2 \n");

  return 0;
}

class MyFunction
{

public:
  int operator()(int x, int y) { return x + y; }
};

int main(int argc, char *argv[])
{

  func fp = print1;
  fp();

  fp = print2;

  fp();

  std::function<int(void)> func(&print1);
  func();

  func = &print2;

  func();

  MyFunction myFunction;
  std::function<int(int, int)> classFunction = myFunction;

  classFunction(1, 2);

  return 0;
}

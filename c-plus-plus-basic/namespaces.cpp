#include "namespaces.h"
#include <iostream>

void mycode::foo()
{
  std::cout << "foo() called in the mycode namespace" << std::endl;
}

using namespace mycode;

int main (int argc, char *argv[]) {
  
  foo();
  return 0;
}

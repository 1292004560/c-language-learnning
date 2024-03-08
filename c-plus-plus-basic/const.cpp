#include <iostream>
#include <cstring>

void mysteryFunction(const std::string * someString)
{
  *someString = "Test";
}

int main (int argc, char *argv[]) {
  std::string myString = "The string";
  mysteryFunction(&myString);
  return 0;
}

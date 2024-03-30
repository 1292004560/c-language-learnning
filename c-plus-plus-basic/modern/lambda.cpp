#include <iostream>
int main(int argc, char *argv[]) {

  int x = 5;
  auto foo = [x](int y) -> int { return x * y; };
  std::cout << foo(8) << std::endl;
  return 0;
}

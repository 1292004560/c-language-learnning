#include <iostream>

int main (int argc, char *argv[]) {
  

  int a = 1, b = 2;

  auto square = [](int x) {return x*x;};

  auto add3 = [a, b](int t) {return a + b +t;};


  auto mul = [](auto a, auto b){return a * b; };

  std::cout << square(add3(3)) << std::endl;

  std::cout << mul(1, 2.3) << std::endl;

  return 0;
}


#include <iostream>
using namespace std;
class Base {
public:
  virtual void f() { std::cout << "Base::f()" << std::endl; };
};
class Derived : public Base {
public:
  virtual void f() override { std::cout << "Derived::f()" << std::endl; };
};

int main(int argc, char *argv[]) {
  Base *d = new Derived();
  auto b = *d;
  b.f();
  auto &c = *d;
  c.f();
  return 0;
}

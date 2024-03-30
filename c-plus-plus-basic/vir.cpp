#include <iostream>
using namespace std;
class A {
public:
  virtual void func() { cout << "A func() called." << endl; }
};
class B : public A {
public:
  void func() { cout << "B func() called." << endl; }
};
int main() {
  A a;
  a.func();
  B b;
  b.func();
  return 0;
}

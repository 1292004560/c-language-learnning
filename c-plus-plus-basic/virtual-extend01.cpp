#include <iostream>
using namespace std;
class A {
public:
  int a;
};
class B : public A {
public:
  int b;
};
class C : public A {
public:
  int c;
};
class D : public B, public C {
public:
  int d;
};
int main() {
  D d;
  d.B::a = 5;
  cout << d.B::a << endl;
  return 0;
}

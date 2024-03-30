#include <iostream>
using namespace std;
class A {
public:
  A() { cout << "A() called." << endl; }
  ~A() { cout << "~A() called." << endl; }
};

class B : public A {
public:
  B() { cout << "B() called." << endl; }
  ~B() { cout << "~B() called." << endl; }
};
int main() {
  B b;
  return 0;
}

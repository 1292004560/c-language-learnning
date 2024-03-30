#include <iostream>
using namespace std;
class Base {
public:
  void func() { cout << "Base func() called." << endl; }
};
class Dervied : public Base {
public:
  void func() { cout << "Dervied func() called." << endl; }
};

int main() {
  Base *b = new Dervied();
  b->func();
  return 0;
}

#include <functional>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

using std::make_pair;
using namespace std;

void f(const string str) { cout << str + str << endl; }

class Base {

public:
  static int StaticFunc(int val) {
    cout << val * val << endl;
    return val * val;
  }
};

class ClassFunc {};

int main(int argc, char *argv[]) {

  function<void(const string)> funcA(f);

  funcA("hello world");

  function<int(int)> funcB(Base::StaticFunc);
  funcB(30);
  return 0;
}

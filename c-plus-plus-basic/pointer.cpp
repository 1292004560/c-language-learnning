#include <iostream>
class foo
{
private:
 int a = 0;


 public:
 void print() { std::cout << a << std::endl; }
 friend class foo_ptr;
};
class foo_ptr
{
private:
 foo* p; //包装的原生指针
public:
 foo_ptr(foo *q) : p(q) {}
 int& operator*() { return p->a; }
 foo* operator->() { return p; }
};
int main()
{
 foo x;
 foo_ptr p = &x;
 std::cout << *p << std::endl;
 *p = 3;
 p->print();
 return 0;
}

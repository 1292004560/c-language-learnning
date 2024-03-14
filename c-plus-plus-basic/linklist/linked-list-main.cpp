#include <iostream>
#include "linked-list.h"
void print(value_t& v) { std::cout << v << ' '; }
int main()
{
 linked_list l{1, 2, 3, 4, 5};
 l.traverse(print);
 l.traverse([](value_t& v) { std::cout << v << ' '; });
 return 0;
}

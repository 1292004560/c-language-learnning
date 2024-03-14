#include <iostream>
#include "linked-list.h"


int main (int argc, char *argv[]) {
  


 value_t a[] = {1, 2, 3, 4, 5};
 linked_list l;
 init(l); //初始化链表（这一步十分重要）
 for (auto e : a) push_back(l, e); //构建链表节点
 for (auto p = l.head; p != nullptr; p = p->next)
 std::cout << p->data << ' ';
 std::cout << std::endl << l._size;
 clear(l);
  return 0;
}

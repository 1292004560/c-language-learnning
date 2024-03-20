#include <iostream>
#include <initializer_list>
// 链表相关类型声明
using value_t = int; // 类型别名
                     //
                     //
                     //
struct _node // 节点类型定义
{
  value_t data;
  _node *next;
};

using node_ptr = _node *;

using callback = void(value_t &);

class linked_list
{

private:
  node_ptr head, tail;
  size_t _size;

public:
  linked_list();
  linked_list(size_t len, value_t *a = nullptr); // a 是默认参数
  linked_list(const std::initializer_list<value_t> &l);
  ~linked_list();
  void push_back(value_t d);
  void clear();
  size_t size();
  void traverse(callback af);
};

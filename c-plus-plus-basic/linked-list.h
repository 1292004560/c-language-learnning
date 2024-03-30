#include <iostream>
// 链表相关类型声明
using value_t = int; // 类型别名
struct _node         // 节点类型定义
{
  value_t data; // 数据域
  _node *next;  // 指针域
};

using node_ptr = _node *; // 类型别名
struct linked_list {
  node_ptr head, tail; // 头尾指针
  size_t _size;        // 节点数目
};
// 链表的操作：原型声明
// 初始化链表
extern void init(linked_list &l);
// 在链表尾部添加数据
extern void push_back(linked_list &l, value_t d);
// 清空链表
extern void clear(linked_list &l);

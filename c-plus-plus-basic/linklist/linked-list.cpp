#include "linked-list.h"
#include <iostream>

linked_list::linked_list() : head(nullptr), tail(nullptr), _size(0) {}

linked_list::linked_list(size_t len, value_t *a) : linked_list()
{
    if (a == nullptr)
        return;
    for (size_t i = 0; i < len; ++i)
        push_back(a[i]);
}

linked_list::linked_list(const std::initializer_list<value_t> &l) : linked_list()
{
    for (auto e : l)
        push_back(e);
}

linked_list::~linked_list() { clear(); }

void linked_list::push_back(value_t d)
{
    node_ptr p = new _node{d, nullptr}; // 分配内存并初始化
    head == nullptr ? head = p : tail->next = p;
    tail = p;
    ++_size;
}

void linked_list::clear()
{
    for (node_ptr p; head != nullptr;)
    {
        p = head;
        head = head->next;
        delete p;
    }
    tail = head; //==nullptr
    _size = 0;
}

void linked_list::traverse(callback af)
{
    for (auto p = head; p != nullptr; p = p->next)
        af(p->data);
}

size_t linked_list::size() { return _size; }

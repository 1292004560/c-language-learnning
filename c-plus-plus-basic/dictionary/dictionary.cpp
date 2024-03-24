#include "dictionary.h"
dictionary::dictionary() : head(nullptr), tail(nullptr), _size(0) {}
dictionary::dictionary(const std::initializer_list<pair>& l) : dictionary()
{
 for (auto e : l) push_back(e);
}
dictionary::~dictionary() { clear(); }
void dictionary::push_back(pair d)
{
 std::hash<std::string> hash_fn;
 node_ptr p = new _node{d, hash_fn(d.key), nullptr};
 if (head == nullptr) head = p;
 else tail->next = p;
 tail = p;
 ++_size;
}


void dictionary::clear() { /*略*/ }
void dictionary::traverse(callback af) { /*略*/ }
value_t& dictionary::operator[](std::string key)
{
 std::hash<std::string> hash_fn;
 for (auto p = head; p != nullptr; p = p->next)
 if (hash_fn(key) == p->hash) return p->data.value;
 throw std::string("key '" + key + "' doesn't exist.");
}




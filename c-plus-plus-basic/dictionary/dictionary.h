#include <iostream>
#include <initializer_list>
#include <functional>


using value_t = int; //类型别名
                     //
struct pair
{
     std::string key;
     value_t value;
};
using callback = void (pair&);



class dictionary
{
private:
 //字典相关类型声明
     struct _node //节点类型定义
     {
         pair data;
         size_t hash; //Hash 值
         _node * next;
     };
     using node_ptr = _node *; //类型别名
     node_ptr head, tail;
     size_t _size;
public:
         dictionary();
         dictionary(const std::initializer_list<pair>& l);
         ~dictionary();
         void push_back(pair d);
         void clear();
         void traverse(callback af); //参数 af 是个函数
         value_t& operator[](std::string key);
};

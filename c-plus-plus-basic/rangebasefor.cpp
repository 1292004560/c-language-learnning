#include <iostream>

int main(int argc, char *argv[])
{

  int a[] = {1, 2, 3, 4, 5}; // 数组是一种区间
  for (auto &&val : a)
    ++val;
  for (auto val : a)
    std::cout << val << ' ';
  std::cout << std::endl;
  for (auto val : {1.2, 2.3, 3.4, 4.5})
    std::cout << val << ' ';
  std::cout << std::endl;
  auto list = {9.8, 8.7, 7.6, 6.5, 5.4}; // 花括号初始化列表也是一种区间
  for (auto val : list)
    std::cout << val << ' ';
  std::cout << std::endl;

  // for (auto val : {1, 2.0}) ++val; //error，花括号初始化列表中的值类型不统一
  // int *p = a;
  // for (auto val : p) --val; //error，for 语句不能用于指向数组元素的指针
  int(*q)[5] = &a; // q 是数组指针，它与 p 完全不同
  for (auto val : *q)
    --val; // OK，q 指向的是数组，而非数组元素

  return 0;
}

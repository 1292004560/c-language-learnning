#include <iostream>
#include "dictionary.h"
int main()
{
 dictionary a{{"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}};
 auto af = [](pair& v) { std::cout << v.key << "=>" << v.value << std::endl; };
 a["four"] = 5;
 a.traverse(af);
 std::cout << a["three"];
 return 0;
}

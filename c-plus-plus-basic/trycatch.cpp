#include <iostream>

int mod(int e1, int e2)
{
 if (e1 == 0) throw int(e1);
 if (e2 == 0) throw long(e2);
 return e1 % e2;
}


int main()
{
 int e1, e2;
 while (true)
 {
 std::cout << "please input two numbers:" << std::endl;
 std::cin >> e1 >> e2;
 if (e1 + e2 == 0) break;
 try //try 块包含在循环中
 {
 std::cout << e1 << " mod " << e2 << " = " << mod(e1, e2) << std::endl;
 }
 catch (int) //因为本异常处理器不需要异常对象的值，所以只需要类型
 {
 std::cout << "It doesn’t make sense when numerator is 0." << std::endl;
 }
 catch (long)
 {
 std::cout << "Denominator cannot be zero." << std::endl;
 }
 }
 std::cout << "It's done." << std::endl;
 return 0;
}

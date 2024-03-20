#include <iostream>

int f() {int x = 3; return x;}

int main (int argc, char *argv[]) {
  

                        int a = 0;
                        //int&& rra = a; //error
                        int&& rra = std::move(a);
                        int&& rrb = f(); //ok
                        ++rra;
                        std::cout << a << ',' << rra << std::endl;
                       return 0;  return 0;
}

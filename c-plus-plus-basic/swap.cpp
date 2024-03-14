#include <iostream>
void swapByVal(int a, int b) {int t = a; a = b; b = t; }
void swapByPtr(int *pa, int *pb) { int t = *pa; *pa = *pb; *pb = t; }
void swapByRef(int &a, int &b) { int t = a; a = b; b = t; }


int main (int argc, char *argv[]) {
  
  int i = 11, j = 99;

  std::cout << "Swap By Value : "<< std::endl;

  std::cout << "i = " << i << ", j = " << std::endl;

  swapByVal(i, j);


  std::cout << "i = " << i << ", j = "<< j << std::endl;

  i = 10, j = 20;

  

  std::cout << "Swap By Reference : "<< std::endl;

  std::cout << "i = " << i << ", j = "<< j << std::endl;

  swapByRef(i, j);

  std::cout << "i = " << i << ", j = "<< j << std::endl;




  i = 10, j = 20;

  

  std::cout << "Swap By Pointer : "<< std::endl;


  std::cout << "i = " << i << ", j = "<< j << std::endl;
  swapByPtr(&i, &j);



  std::cout << "i = " << i << ", j = "<< j << std::endl;
  return 0;
}

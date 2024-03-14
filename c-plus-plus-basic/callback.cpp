#include <iostream>
#include <cstdlib>
#include <ctime>

void generator(unsigned &v){v = rand() % 100;}

void arrayWalk(unsigned * a, size_t len, void callback(unsigned &))
{
  for (size_t i = 0; i < len; ++i) callback(a[i]);
}


bool gt50(unsigned v){return v > 50;}

size_t countif(unsigned *a, size_t len, bool pred(unsigned))
{
 size_t count = 0;
 for (size_t i = 0; i < len; ++i) if (pred(a[i])) ++count;
 return count;
}


int main (int argc, char *argv[]) {
 
  srand(time(0));
  const size_t len = 5;
  unsigned a[len];
  //fill array
  arrayWalk(a, len, generator);
 //print array, using lambda
  arrayWalk(a, len, [](unsigned& v) { std::cout << v << ' '; });
  std::cout << "\nCount of greater than 60: " << countif(a, len, gt50);
  std::cout << "\nCount of greater than 20: " << countif(a, len, [](unsigned v){return v > 20;});

  return 0;
}

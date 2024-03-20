#include <iostream>
#include <iomanip>

class complex
{
private:
   double real, imag;
public:
   complex(double r = 0, double i = 0) : real(r), imag(i) {}
   friend void print(const complex& c);
   friend complex operator+(const complex& a, const complex& b);
   friend complex operator-(const complex &a, const complex &b);
};

void print(const complex& c)
{
   std::cout << std::setprecision(2) << c.real << " + i" << c.imag << std::endl;
}

complex operator+(const complex& a, const complex& b)
{
   return complex{a.real + b.real, a.imag + b.imag};
}
complex operator-(const complex &a, const complex &b)
{
   return complex{a.real - b.real, a.imag - b.imag};
}

int main()
{
   complex c1{1.2, 2.3}, c2{3.4, 4.5}, c3;
   c3 = c1 + c2;
   print(c3);
   print(c3 -c1);
   return 0;
}

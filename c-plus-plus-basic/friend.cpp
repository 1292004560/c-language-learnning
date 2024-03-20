#include <iostream>
class X
{




private:
 int i = 1;



public:
 int get() { return i; }
 friend int set(X&, int);
 friend class Y;


};


int set(X& o, int j) { return o.i = j; }


class Y
{



public:
 int set(X& o, int j) { return o.i = j; }




};


int main()
{


 X x;
 set(x, 2);
 std::cout << x.get();


 Y y;
 y.set(x, 3);
 std::cout << ' ' << x.get();
 return 0;
}

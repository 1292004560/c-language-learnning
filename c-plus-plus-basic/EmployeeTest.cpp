#include <iostream>
#include "Employee.h"

using namespace std;
using namespace Records;

int main()
{
  cout << "Testing the Employee class." << endl;

  Employee emp;

  emp.setFirstName("John");

  emp.setLastName("Doe");
  emp.display();

  return 0;
}

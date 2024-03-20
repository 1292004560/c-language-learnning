#include <iostream>
#include "Employee.h"

using namespace std;

namespace Records
{

  Employee::Employee(const std::string &firstName, const std::string &lastName) : mFirstName(firstName), mLastName(lastName)
  {
  }

  void Employee::promote(int raiseAccount)
  {
    setSalary(getSalary() + raiseAccount);
  }

  void Employee::demote(int demeritAccount)
  {
    setSalary(getSalary() - demeritAccount);
  }

  void Employee::hire()
  {
    mHired = true;
  }

  void Employee::fire()
  {
    mHired = false;
  }

  void Employee::display() const
  {
    cout << "Hello world" << getLastName() << getFirstName();
  }

  void Employee::setFirstName(const string &firstName)
  {
    mFirstName = firstName;
  }

  const string &Employee::getFirstName() const
  {
    return mFirstName;
  }
}

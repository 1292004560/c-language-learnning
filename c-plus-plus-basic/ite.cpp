#include <iostream>
#include <list>

class Counter
{
public:
  Counter(int low, int high)
  {
    for (int i = low; i <= high; i++)
    {
      numbers.push_back(i);
    }
  }

  std::list<int>::iterator begin() { return numbers.begin(); }

  std::list<int>::iterator end() { return numbers.end(); }

private:
  std::list<int> numbers;
};

int main()
{
  Counter counter(5, 10);

  for (auto it = counter.begin(); it != counter.end(); ++it)
  {
    std::cout << *it << std::endl;
  }

  return 0;
}

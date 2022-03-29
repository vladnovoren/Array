#include "array.hpp"
#include <iostream>
#include <ctime>

int main() {
  try {
    srand(time(NULL));
    Array<int> array(10000000000);
  } catch (std::bad_alloc& e) {
    std::cout << "Array allocation failed: " << e.what() << '\n';
  }
  return 0;
}

#include "array.hpp"
#include <iostream>

int main() {
  try {
    Array<int> array(100000000000);
  } catch (std::bad_alloc& e) {
    std::cout << "Array allocation failed: " << e.what() << '\n';
  }
  return 0;
}

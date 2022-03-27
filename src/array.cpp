#include "array.hpp"
#include <iostream>
#include <ctime>

int main() {
  try {
    srand(time(NULL));
    Array<int> array(100);
    for (size_t i = 0; i < 1024; ++i) {
      size_t new_size = rand() % 1000000;
      array.Resize(new_size);
    }
  } catch (std::bad_alloc& e) {
    std::cout << "Array allocation failed: " << e.what() << '\n';
  }
  return 0;
}

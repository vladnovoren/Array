#include "dynamic_storage.hpp"
#include <iostream>

int main() {
  try {
    DynamicStorage<DynamicStorage<int>> matrix;
    for (size_t i = 0; i < 1000; ++i) {
      matrix.Resize(rand() % 10000);
    }

    // DynamicStorage<DynamicStorage<int>> matrix;
    // matrix.Resize(100);
  } catch (std::bad_alloc& e) {
    std::cout << "Array allocation failed: " << e.what() << '\n';
  }
  return 0;
}

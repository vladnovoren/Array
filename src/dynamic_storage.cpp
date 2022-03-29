#include "dynamic_storage.hpp"
#include <iostream>

int main() {
  srand(time(NULL));
  try {
    DynamicStorage<DynamicStorage<int>> matrix;
    // printf("p: %p\n", &matrix);
    for (size_t i = 0; i < 1000; ++i) {
      matrix.Resize(rand() % 10000);
    }

    for (size_t i = 0; i < 9; ++i) {
      matrix.PushBack(DynamicStorage<int>());
    }


    for (size_t i = 0; i < 1000; ++i) {
      matrix.PushBack(DynamicStorage<int>());
    }

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

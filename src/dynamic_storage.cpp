#include "dynamic_storage.hpp"
#include <iostream>

int main() {
  srand(time(NULL));
  try {
    for (size_t i = 0; i < 100; ++i) {
      DynamicStorage<DynamicStorage<int>> matrix;
      for (size_t i = 0; i < 1000; ++i) {
        matrix.Resize(rand() % 10000);
      }

      for (size_t i = 0; i < 1000; ++i) {
        matrix.PushBack(DynamicStorage<int>(100));
      }

      for (size_t i = 0; i < 1000; ++i) {
        matrix.Resize(rand() % 10000);
      }

      for (size_t i = 0; i < 1000; ++i) {
        matrix.PushBack(DynamicStorage<int>(100));
      }
    }
  } catch (std::bad_alloc& e) {
    std::cout << "Array allocation failed: " << e.what() << '\n';
  } catch (std::runtime_error& e) {
    std::cout << "Runtime error: " << e.what() << '\n';
  }
  return 0;
}

#include "dynamic_storage.hpp"
#include <iostream>

int main() {
  try {
    DynamicStorage<int> storage(1000);
    for (size_t i = 0; i < 1000; ++i) {
      storage[i] = i;
    }
    for (size_t i = 0; i < 1000; ++i) {
      storage.Resize(rand() % 1000000);
    }
    for (int i = 0; i < 1000; ++i) {
      storage.PushBack(1);
    }
    for (int i = 0; i < 1000; ++i) {
      storage.PopBack();
    }
    DynamicStorage<int> copy(storage);
  } catch (std::bad_alloc& e) {
    std::cout << "Array allocation failed: " << e.what() << '\n';
  }
  return 0;
}

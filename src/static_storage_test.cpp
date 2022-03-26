#include "dynamic_storage.hpp"
#include <iostream>

int main() {
  DynamicStorage<int> storage(10);
  for (int i = 0; i < 10; ++i) {
    std::cout << storage[i] << ' ';
  }
  std::cout << '\n';
  return 0;
}

#include "array.hpp"
#include <iostream>

int main() {
  srand(time(NULL));
  try {
    Array<int, StaticStorage, 1000> array;

    array.Resize(100);

    for (size_t i = 0; i < 100; ++i) {
      array[i] = i;
    }

    for (size_t i = 0; i < 100; ++i) {
      array.PopBack();
    }

  } catch (std::bad_alloc& e) {
    std::cout << "Array allocation failed: " << e.what() << '\n';
  } catch (std::runtime_error& e) {
    std::cout << "Runtime error: " << e.what() << '\n';
  } catch (std::exception& e) {
    std::cout << "Some error: " << e.what() << '\n';
  }
  return 0;
}

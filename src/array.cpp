#include "array.hpp"
#include <iostream>
#include <vector>
#include <ctime>
#include "chunked_storage.hpp"

struct Point {
  Point() {}
  Point(int x, int y) : x_{x}, y_{y} {}

  int x_ = 0;
  int y_ = 0;
};

void BoolTest() {
  Array<bool> arr(10);

  for (size_t i = 0; i < 10; ++i) {
    arr[i] = i % 2;
  }

  for (size_t i = 0; i < 10; ++i) {
    printf("%d ", bool(arr[i]));
  }
  printf("\n");
}

void CommonTest() {
  try {
    Array<Array<int>> matrix;
    matrix.PushBack(Array<int>());

    for (size_t i = 0; i < 10; ++i) {
      matrix.PushBack(Array<int>());
      matrix.Shrink();
      matrix[i].Shrink();
    }

    Array<int> array(100);
    for (size_t i = 0; i < 100; ++i) {
      array.PushBack(1);
    }

    printf("%zu\n", matrix.Size());
    printf("%zu\n", matrix[0].Size());


    Array<Point> arr;
    arr.EmplaceBack(1, 2);
    Point a = arr.Front();
    printf("%d %d\n", a.x_, a.y_);
  } catch (std::bad_alloc& e) {
    std::cout << "Array allocation failed: " << e.what() << '\n';
  } catch (std::runtime_error& e) {
    std::cout << "Runtime error: " << e.what() << '\n';
  } catch (std::logic_error& e) {
    std::cout << "Logic error: " << e.what() << '\n';
  }
}

int main() {
  srand(time(NULL));
  CommonTest();
  BoolTest();
  ChunkedStorage<int> storage;
  printf("%zu\n", ChunkedStorage<int>::CHUNK_SIZE_);
  return 0;
}

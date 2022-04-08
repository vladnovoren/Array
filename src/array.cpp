#include "array.hpp"
#include <iostream>
#include <ctime>

struct Point {
  Point() {}
  Point(int x, int y) : x_{x}, y_{y} {}

  int x_ = 0;
  int y_ = 0;
};

void BoolTest() {
  Array<bool> arr(10);
  for (size_t i = 0; i < 10; ++i) {
    arr[i] = (i % 2);
    arr[i] = arr[0];
    printf("%d ", (bool)arr[i]);
  }
  printf("\n");

  Array<bool> arr1(10);
  arr1[0] = arr[0];
  Array<bool> arr2(std::move(arr));
  arr1[0] = false;
}

void CommonTest() {
  try {
    for (size_t i = 0; i < 1; ++i) {
      Array<Array<int>> matrix;

      for (size_t i = 0; i < 1000; ++i) {
        matrix.PushBack(Array<int>(100));
        matrix.Shrink();
        matrix[i].Shrink();
      }

      printf("%zu\n", matrix.Size());
      printf("%zu\n", matrix[0].Size());

      int a = matrix[0].At(0);
      printf("%d\n", a);
    }

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
  return 0;
}

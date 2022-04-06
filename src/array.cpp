#include "array.hpp"
#include <iostream>
#include <ctime>

struct Point {
  Point() {}
  Point(int x, int y) : x_{x}, y_{y} {}

  int x_ = 0;
  int y_ = 0;
};

int main() {
  srand(time(NULL));
  try {
    for (size_t i = 0; i < 1; ++i) {
      Array<Array<int>> matrix;

      for (size_t i = 0; i < 1000; ++i) {
        matrix.PushBack(Array<int>(100));
        matrix.Shrink();
        matrix[i].Shrink();
      }
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
  return 0;
}

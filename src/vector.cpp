#include "vector.hpp"
#include <iostream>
#include <vector>
#include <ctime>

struct Point {
  Point() {}
  Point(int x, int y) : x_{x}, y_{y} {}

  int x_ = 0;
  int y_ = 0;
};

void BoolTest() {
  Vector<bool> arr(10);

  for (size_t i = 0; i < 10; ++i) {
    arr[i] = i % 2;
  }

  for (size_t i = 0; i < 10; ++i) {
    printf("%d ", bool(arr[i]));
  }
  printf("\n");

  Vector<bool> by_list = {true, false, true, false, true};
  for (size_t i = 0; i < by_list.Size(); ++i) {
    std::cout << std::boolalpha << by_list[i] << ' ';
  }
  std::cout << '\n';
}

void CommonTest() {
  try {
    Vector<Vector<int>> matrix;
    matrix.PushBack(Vector<int>());

    for (size_t i = 0; i < 10; ++i) {
      matrix.PushBack(Vector<int>());
      matrix.Shrink();
      matrix[i].Shrink();
    }

    Vector<int> vector(100);
    for (size_t i = 0; i < 100; ++i) {
      vector.PushBack(1);
    }

    printf("%zu\n", matrix.Size());
    printf("%zu\n", matrix[0].Size());

    Vector<Point> arr;
    arr.EmplaceBack(1, 2);
    Point a = arr.Front();
    printf("%d %d\n", a.x_, a.y_);

    Vector<int> by_list = {1, 2, 3, 4, 5};
    for (size_t i = 0; i < by_list.Size(); ++i) {
      std::cout << by_list[i] << ' ';
    }
    std::cout << '\n';
  } catch (std::bad_alloc& e) {
    std::cout << "Vector allocation failed: " << e.what() << '\n';
  } catch (std::runtime_error& e) {
    std::cout << "Runtime error: " << e.what() << '\n';
  } catch (std::logic_error& e) {
    std::cout << "Logic error: " << e.what() << '\n';
  }
}

void TestChunkedStorage() {
  try {
    Vector<Vector<int, ChunkedStorage>, ChunkedStorage> matrix;
    matrix.PushBack(Vector<int, ChunkedStorage>());

    for (size_t i = 0; i < 10; ++i) {
      matrix.PushBack(Vector<int, ChunkedStorage>());
      matrix.Shrink();
      matrix[i].Shrink();
    }

    Vector<int> vector(100);
    for (size_t i = 0; i < 100; ++i) {
      vector.PushBack(1);
    }

    printf("%zu\n", matrix.Size());
    printf("%zu\n", matrix[0].Size());

    Vector<Point> arr;
    arr.EmplaceBack(1, 2);
    Point a = arr.Front();
    printf("%d %d\n", a.x_, a.y_);

    Vector<int> by_list = {1, 2, 3, 4, 5};
    for (size_t i = 0; i < by_list.Size(); ++i) {
      std::cout << by_list[i] << ' ';
    }
    std::cout << '\n';

  } catch (std::bad_alloc& e) {
    std::cout << "Vector allocation failed: " << e.what() << '\n';
  } catch (std::runtime_error& e) {
    std::cout << "Runtime error: " << e.what() << '\n';
  } catch (std::logic_error& e) {
    std::cout << "Logic error: " << e.what() << '\n';
  }
}

void TestIterators() {
  Vector<int> v_arr = {1, 2, 3, 4, 5};
  auto v_it = v_arr.begin();

  const Vector<int> c_arr = {1, 2, 3, 4, 5};
  auto c_it = c_arr.cend();
  c_it--;
  --c_it;

  std::cout << *v_it << ' ' << *c_it << '\n';

  c_it = v_it;
  v_it += 2;

  std::cout << v_it[2] << '\n';

}

int main() {
  srand(time(NULL));

  CommonTest();
  BoolTest();
  TestChunkedStorage();

  TestIterators();

  return 0;
}

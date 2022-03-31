#include <iostream>
#include <vector>

class NonMovable {
 public:
  NonMovable() = default;

  // NonMovable(const NonMovable& other) = default;
  // NonMovable(NonMovable&& other) = delete;

  NonMovable(const NonMovable& other) = delete;
  NonMovable(NonMovable&& other) = default;

  ~NonMovable() = default;
  
  // NonMovable& operator=(const NonMovable& other) = default;
  // NonMovable& operator=(NonMovable&& other) = delete;

  NonMovable& operator=(const NonMovable& other) = delete;
  NonMovable& operator=(NonMovable&& other) = default;

  int x{0};
  int y{0};
};

int main() {
  std::vector<NonMovable> vec;
  for (size_t i = 0; i < 1000; ++i) {
    vec.push_back(NonMovable());
  }
  std::vector<NonMovable> vec2(vec);
  return 0;
}
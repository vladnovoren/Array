cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_C_COMPILER clang-12)
set(CMAKE_CXX_COMPILER clang++-12)
set(CMAKE_CXX_STANDARD 20)

project(Vector)

add_compile_options(
  -Wall
  -Wextra

  -Werror

  -Og
  -g
  -fsanitize=address
)

add_link_options(
  -Og
  -g
  -fsanitize=address
)

add_executable(vector src/vector.cpp)
target_include_directories(vector PUBLIC include/)

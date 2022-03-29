cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)

project(Array)

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

add_executable(array src/array.cpp)
add_executable(dynamic_storage src/dynamic_storage.cpp)
add_executable(static_storage src/static_storage.cpp)

target_include_directories(array PUBLIC include/)
target_include_directories(dynamic_storage PUBLIC include/)
target_include_directories(static_storage PUBLIC include/)

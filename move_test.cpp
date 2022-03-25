#include <iostream>

int main() {
  char* array = new char[100]{'r'};
  for (size_t i = 0; i < 100; ++i) {
    printf("%c", array[i]);
  }
  printf("\n");
  return 0;
}
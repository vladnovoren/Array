#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "dynamic_storage.hpp"

template<
  typename ElemT,
  template <typename StorageT, size_t StorageSize> class Storage = DynamicStorage,
  size_t N = 0
>
class Array : public Storage<ElemT, N> {
 public:
  Array() {
  }

  Array(const size_t size) : Storage<ElemT, N>(size) {
  }

  Array(const Array& other) = default;

  Array(Array&& other) = default;

 private:

};

#endif /* array.hpp */

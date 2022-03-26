#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "dynamic_storage.hpp"

template<
  typename ElemT,
  template <typename StorageT, size_t StorageSize> class Storage = DynamicStorage,
  size_t N = 0
>
class Array {
 public:
  Array() {
  }

  Array(const size_t size) : storage_(size) {
  }

  Array(const Array& other) = default;

  Array(Array&& other) = default;

  

 private:
  DynamicStorage<ElemT> storage_;

};

#endif /* array.hpp */

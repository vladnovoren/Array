#ifndef ARRAY_HPP
#define ARRAY_HPP

template<
  typename ElemT,
  template <typename StorageT, size_t StorageSize> class Storage = DynamicStorage,
  size_t size_ = 0
>
class Array {
 public:
  Array(): storage_() {
  }

  Array(const size_t size): storage_(size) {
  }

  Array(const Array& other) = default;

  Array(Array&& other) = default;  

 private:
  Storage<ElemT, size_> storage_;
};

#endif /* array.hpp */

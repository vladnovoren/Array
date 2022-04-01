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

  ~Array() {
  }

  Array& operator=(const Array& other) {
    Array tmp(other);
    std::swap(*this, tmp);
    return *this;
  }

  Array& operator=(Array&& other) {
    Array tmp(other);
    std::swap(storage_, other.storage_);
    return *this;
  }

  ElemT& operator[](const size_t index) {
    if (index >= Size()) {
      throw std::runtime_error(BAD_INDEX_MSG_);
    }

    return storage_.Buffer()[index];
  }

  const ElemT& operator[](const size_t index) const {
    if (index >= Size()) {
      throw std::runtime_error(BAD_INDEX_MSG_);
    }

    return storage_.Buffer()[index];
  }

  [[nodiscard]] inline size_t Size() const {
    return storage_.Size();
  }

  void Resize(const size_t new_size) {
    storage_.Resize(new_size);
  }

  void PushBack(ElemT new_elem) {
    storage_.PushBack(std::move(new_elem));
  }

  void PopBack() {
    storage_.PopBack();
  }

 private:
  Storage<ElemT, N> storage_;

};

#endif /* array.hpp */

#ifndef STATIC_STORAGE_HPP
#define STATIC_STORAGE_HPP

#include <cstddef>
#include <cassert>
#include <utility>
#include <cstdint>
#include <new>

template<
  typename ElemT,
  size_t size_
>
class StaticStorage {
 public:
  StaticStorage() : data_(static_cast<ElemT*>(buffer_)) {
    for (size_t i = 0; i < size_; ++i) {
      new (data_ + i) ElemT();
    }
  }

  StaticStorage(const StaticStorage& other) : data_(static_cast<ElemT*>(buffer_)) {
    for (size_t i = 0; i < size_; ++i) {
      new (data_ + i) ElemT(other.data_[i]);
    }
  }

  StaticStorage(StaticStorage&& other) : data_(static_cast<ElemT*>(buffer_)) {
    for (size_t i = 0; i < size_; ++i) {
      new (data_ + i) ElemT(std::move(other.data_[i]));
    }
  }

  ~StaticStorage() {
    for (size_t i = 0; i < size_; ++i) {
      data_[i].~ElemT();
    }
  }

  size_t Size() const {
    return size_;
  }

 private:
  ElemT* data_;
  uint8_t buffer_[size_ * sizeof(ElemT)];

};

#endif /* static_storage.hpp */

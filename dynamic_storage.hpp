#ifndef DYNAMIC_STORAGE_HPP
#define DYNAMIC_STORAGE_HPP

#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <utility>
#include <cstdint>
#include <new>
#include "istorage.hpp"

size_t UpperBinPower(const size_t a) {
  size_t res = 1;
  while (res < a) {
    res <<= 1;
  }
  return res;
}

template<
  typename ElemT,
  size_t N = 0
>
class DynamicStorage : IStorage<ElemT, N> {
 public:
  DynamicStorage() {
  }

  DynamicStorage(const size_t size) : size_(size) {
    capacity_ = CountCapacity(size_);

    buffer_ = static_cast<ElemT*>(new(capacity_ * sizeof(ElemT)));
    if (buffer_ == nullptr) {
      throw std::bad_alloc();
    }

    PlaceDefault();
  }

  DynamicStorage(const DynamicStorage& other) : size_(other.size_), capacity_(other.capacity_) {
    buffer_ = static_cast<ElemT*>(new(capacity_ * sizeof(ElemT)));
    if (buffer_ == nullptr) {
      throw std::bad_alloc();
    }

    PlaceFromOther(other);
  }

  DynamicStorage(DynamicStorage&& other) {
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    std::swap(buffer_, other.buffer_);
  }

  ~DynamicStorage() override {
    delete[] buffer_;
  }

  DynamicStorage& operator=(const DynamicStorage& other) {
    size_ = other.size_;
    capacity_ = other.capacity_;

    PlaceFromOther(other);

    return *this;
  }

  DynamicStorage& operator=(DynamicStorage&& other) {
    delete[] buffer_;

    size_ = other.size_;
    other.size_ = 0;

    capacity_ = other.capacity_;
    other.capacity_ = 0;

    buffer_ = other.buffer_;
    other.buffer_ = nullptr;

    return *this;
  }

  size_t Size() const override {
    return size_;
  }

  void Resize(const size_t new_size) {
    size_ = new_size;
    UpdateCapacity();
    UpdateBufferWithNewCapacity();
  }

 private:
  void UpdateCapacity() {
    size_t req_capacity = CountCapacity();
    if (req_capacity <= capacity_) {
      return;
    }
    capacity_ = req_capacity;
  }

  void UpdateBufferWithNewCapacity() {
    ElemT* old_data = buffer_;
    buffer_ = static_cast<ElemT*>(new(capacity_ * sizeof(ElemT)));
    if (buffer_ == nullptr) {
      throw std::bad_alloc();
    }

    PlaceFromOtherBuffer(old_data);
    Destruct(0, )
  }

  size_t CountCapacity(const size_t size) {
    if (size <= MIN_CAPACITY) {
      return MIN_CAPACITY;
    } else {
      return UpperBinPower(size);
    }
  }

  void PlaceDefault() {
    for (size_t i = 0; i < size; ++i) {
      new (buffer_ + i) ElemT();
    }
  }

  void PlaceFromOther(const DynamicStorage& other) {
    PlaceFromOtherBuffer(other.buffer_);
  }

  void PlaceFromOtherBuffer(const ElemT* other) {
    for (size_t i = 0; i < size_; ++i) {
      new (buffer_ + i) ElemT(other[i]);
    }
  }

  void DestructOne(const size_t index) {
    buffer_[index].~ElemT();
  }

  void Destruct(const size_t left, const size_t right) {
    for (size_t i = left; i <= right; ++i) {
      DestructOne(i);
    }
  }

 private:
  static const size_t MIN_CAPACITY = 8;

  ElemT* buffer_{nullptr};

  size_t size_{0};
  size_t capacity_{0};

};

#endif /* dynamic_storage.hpp */

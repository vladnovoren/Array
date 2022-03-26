#ifndef DYNAMIC_STORAGE_HPP
#define DYNAMIC_STORAGE_HPP

#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <utility>
#include <cstdint>
#include <new>
#include <algorithm>
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
class DynamicStorage : public IStorage<ElemT, N> {
 public:
  DynamicStorage() {
  }

  DynamicStorage(const size_t size) : size_(size) {
    capacity_ = CountCapacity(size_);

    try {
      this->buffer_ = static_cast<ElemT*>(operator new(capacity_ * sizeof(ElemT)));
    } catch (const std::bad_alloc& e) {
      throw;
    }

    PlaceDefault();
  }

  DynamicStorage(const DynamicStorage& other) : size_(other.size_), capacity_(other.capacity_) {
    try {
      this->buffer_ = static_cast<ElemT*>(operator new(capacity_ * sizeof(ElemT)));
    } catch (const std::bad_alloc& e) {
      throw;
    }

    PlaceFromOther(other);
  }

  DynamicStorage(DynamicStorage&& other) {
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    std::swap(this->buffer_, other.buffer_);
  }

  ~DynamicStorage() override {
    DestructBuffer(this->buffer_, 0, size_ - 1);
  }

  DynamicStorage& operator=(const DynamicStorage& other) {
    size_ = other.size_;
    capacity_ = other.capacity_;

    PlaceFromOther(other);

    return *this;
  }

  DynamicStorage& operator=(DynamicStorage&& other) {
    DestructBuffer(this->buffer_, 0, size_ - 1);

    size_ = other.size_;
    other.size_ = 0;

    capacity_ = other.capacity_;
    other.capacity_ = 0;

    this->buffer_ = other.buffer_;
    other.buffer_ = nullptr;

    return *this;
  }

  size_t Size() const override {
    return size_;
  }

  void Resize(const size_t new_size) {
    if (UpdateCapacity(new_size)) {
      UpdateBufferWithNewCapacity(new_size);
    }
  }

 private:
  bool UpdateCapacity(const size_t new_size) {
    size_t req_capacity = CountCapacity(new_size);
    if (req_capacity <= capacity_) {
      return false;
    }
    capacity_ = req_capacity;
    return true;
  }

  void UpdateBufferWithNewCapacity(const size_t new_size) {
    ElemT* old_buffer = this->buffer_;
    try {
      this->buffer_ = static_cast<ElemT*>(operator new(capacity_ * sizeof(ElemT)));
    } catch (const std::bad_alloc& e) {
      throw;
    }

    size_t min_size = std::min(size_, new_size);
    PlaceDefault()
    if (new_size > size_) {
      PlaceDefault(size_ + 1, new_size - 1);
    } else {

    }
    DestructBuffer(old_buffer, 0, size_);
    size_ = new_size;
  }

  size_t CountCapacity(const size_t size) {
    if (size <= MIN_CAPACITY) {
      return MIN_CAPACITY;
    } else {
      return UpperBinPower(size);
    }
  }

  void PlaceDefault(const size_t left, const size_t right) {
    for (size_t i = left; i <= right; ++i) {
      new (this->buffer_ + i) ElemT();
    }
  }

  void PlaceFromOther(const DynamicStorage& other) {
    PlaceFromOtherBuffer(other.buffer_);
  }

  void PlaceFromOtherBuffer(const ElemT* other, const size_t left, const size_t right) {
    for (size_t i = left; i <= right; ++i) {
      new (this->buffer_ + i) ElemT(other[i]);
    }
  }

  void DestructBuffer(ElemT* buffer, const size_t left, const size_t right) {
    assert(buffer != nullptr);

    for (size_t i = left; i <= right; ++i) {
      buffer[i].~ElemT();
    }
    operator delete(buffer);
  }

 private:
  static const size_t MIN_CAPACITY = 8;

  size_t size_{0};
  size_t capacity_{0};

};

#endif /* dynamic_storage.hpp */

#ifndef DYNAMIC_STORAGE_HPP
#define DYNAMIC_STORAGE_HPP

#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <utility>
#include <cstdint>
#include <new>
#include <algorithm>
#include <iostream>
#include "object_helpers.hpp"

static const char* const BAD_POP_MSG_ = "attempt to remove the last element of an empty vector";
static const char* const BAD_INDEX_MSG_ = "trying to access on vector with invalid index";

template<
  typename ElemT,
  size_t N = 0
>
class DynamicStorage {
 public:
  DynamicStorage() : size_(0), capacity_(MIN_CAPACITY),
    buffer_{static_cast<ElemT*>(::operator new(MIN_CAPACITY * sizeof(ElemT)))} {
  }

  DynamicStorage(const size_t size) : size_(size), capacity_(size),
    buffer_{static_cast<ElemT*>(::operator new(size * sizeof(ElemT)))} {
    DefaultConstruct(buffer_, size_);
  }

  DynamicStorage(const DynamicStorage& other) {
    buffer_ = SafeCopy(other.buffer_, other.size_, other.size_);
    size_ = capacity_ = other.size_;
  }

  DynamicStorage(DynamicStorage&& other) {
    std::swap(this->size_, other.size_);
    std::swap(this->capacity_, other.capacity_);
    std::swap(this->buffer_, other.buffer_);
  }

  ~DynamicStorage() {
    if (buffer_ != nullptr) {
      DestructAndDelete(buffer_, size_);
    }
    size_ = 0;
    capacity_ = 0;
  }

  DynamicStorage& operator=(const DynamicStorage& other) {
    if (this == &other) {
      return *this;
    }

    DynamicStorage tmp(other);
    std::swap(*this, tmp);
    return *this;
  }

  DynamicStorage& operator=(DynamicStorage&& other) {
    if (this == &other) {
      return *this;
    }

    DynamicStorage tmp(other);
    std::swap(buffer_, tmp.buffer_);
    std::swap(size_, tmp.size_);
    std::swap(capacity_, tmp.capacity_);

    return *this;
  }

  [[nodiscard]] size_t Size() const {
    return size_;
  }

  void Resize(const size_t new_size) {
    if (size_ == new_size) {
      return;
    }

    if (new_size <= capacity_) {
      if (new_size < size_) {
        Destruct(buffer_, new_size, size_);
      } else {
        while (size_ < new_size) {
          DefaultConstruct(buffer_ + size_);
          ++size_;
        }
      }
    } else if (size_ == capacity_ && new_size == size_ + 1) {
      DoubleBuffer();
      DefaultConstruct(buffer_ + size_);
    } else {
      IncreaseBuffer(new_size);
    }

    size_ = new_size;
  }

  template<typename OtherT>
  void PushBack(OtherT&& new_elem) {
    assert(size_ <= capacity_);

    Resize(size_ + 1);
    buffer_[size_ - 1] = std::forward<OtherT>(new_elem);
  }

  void PopBack() {
    if (size_ == 0) {
      throw std::runtime_error(BAD_POP_MSG_);
    }

    Destruct(&buffer_[size_ - 1]);
    --size_;
  }

  [[nodiscard]] inline ElemT* Buffer() {
    return buffer_;
  }

  [[nodiscard]] inline const ElemT* Buffer() const {
    return buffer_;
  }

  void DoubleBuffer() {
    ElemT* old_buffer = buffer_;

    const size_t new_capacity = 2 * size_ + 1;

    buffer_ = SafeMove(old_buffer, new_capacity, size_);
    DestructAndDelete(old_buffer, size_);
    capacity_ = new_capacity;
  }

  void IncreaseBuffer(const size_t new_size) {
    assert(new_size > capacity_);
    assert(new_size > size_);

    ElemT* old_buffer = buffer_;
    buffer_ = SafeMove(old_buffer, new_size, size_);
    DefaultConstruct(buffer_, size_, new_size);
    DestructAndDelete(old_buffer, size_);
    capacity_ = new_size;
  }

 public:
  static const size_t MIN_CAPACITY = 8;

  size_t size_{0};
  size_t capacity_{0};

  ElemT* buffer_{nullptr};

};

#endif /* dynamic_storage.hpp */

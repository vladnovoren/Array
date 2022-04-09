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
#include "error_msgs.hpp"

template<
  typename ElemT,
  size_t N = 0
>
class DynamicStorage {
 public:
  DynamicStorage() :
    buffer_{static_cast<ElemT*>(::operator new(DEFAULT_CAPACITY * sizeof(ElemT)))},
    capacity_{DEFAULT_CAPACITY} {
  }

  DynamicStorage(const size_t size) : 
    buffer_{static_cast<ElemT*>(::operator new(size * sizeof(ElemT)))},
    capacity_{size},
    size_{DefaultConstruct(buffer_, size)} {
  }

  template<typename ArgT>
  DynamicStorage(const size_t size, ArgT&& arg) :
    buffer_{static_cast<ElemT*>(::operator new(size * sizeof(ElemT)))},
    capacity_{size},
    size_{Construct(buffer_, size, arg)} {
  }

  DynamicStorage(const DynamicStorage& other_copy) :
    buffer_{SafeCopy(other_copy.buffer_, other_copy.size_, other_copy.size_)},
    capacity_(other_copy.size_), size_{other_copy.size_} {
  }

  DynamicStorage(DynamicStorage&& other_move) {
    printf("move constructor\n");
    this->SwapFields(other_move);
  }

  ~DynamicStorage() {
    if (buffer_ != nullptr) {
      printf("size: %zu\n", size_);
      DestructAndDelete(buffer_, size_);
    }

    size_ = 0;
    capacity_ = 0;
  }

  DynamicStorage& operator=(const DynamicStorage& other_copy) {
    if (this == &other_copy) {
      return *this;
    }

    DynamicStorage tmp(other_copy);
    std::swap(*this, tmp);
    return *this;
  }

  DynamicStorage& operator=(DynamicStorage&& other_move) {
    if (this == &other_move) {
      return *this;
    }

    printf("move assignment\n");
    this->SwapFields(other_move);
    return *this;
  }

  [[nodiscard]] inline size_t Size() const {
    return size_;
  }

  [[nodiscard]] inline size_t Capacity() const {
    return capacity_;
  }

  [[nodiscard]] inline ElemT& At(const size_t index) {
    return buffer_[index];
  }

  [[nodiscard]] inline const ElemT& At(const size_t index) const {
    return buffer_[index];
  }

  void Resize(const size_t new_size, bool construct = true) {
    if (size_ == new_size) {
      return;
    }

    if (new_size <= capacity_) {
      if (new_size < size_) {
        Destruct(buffer_, new_size, size_);
      } else {
        if (construct) {
          while (size_ < new_size) {
            DefaultConstruct(buffer_ + size_);
            ++size_;
          }
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

  void RequireCapacity(const size_t at_least_cpcty) {
    if (at_least_cpcty <= capacity_) {
      return;
    }

    if (at_least_cpcty )
  }

  void Shrink() {
    if (capacity_ == size_) {
      return;
    }

    ElemT* old_buffer_ = buffer_;
    if (size_ == 0) {
      buffer_ = static_cast<ElemT*>(::operator new(DEFAULT_CAPACITY * sizeof(ElemT)));
      capacity_ = DEFAULT_CAPACITY;
    } else {
      buffer_ = SafeMove(old_buffer_, size_, size_);
      capacity_ = size_;
    }

    DestructAndDelete(old_buffer_, size_);
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
  void SwapFields(DynamicStorage& other) {
    std::swap(buffer_, other.buffer_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
  }

  static const size_t DEFAULT_CAPACITY = 8;

  ElemT* buffer_{nullptr};

  size_t capacity_{0};
  size_t size_{0};

};

#endif /* dynamic_storage.hpp */

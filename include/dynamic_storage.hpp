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
#include "istorage.hpp"
#include "object_helpers.hpp"

template<
  typename ElemT,
  size_t N = 0
>
class DynamicStorage : public IStorage<ElemT, N> {
  using IStorage<ElemT, N>::buffer_;

 public:
  DynamicStorage() : IStorage<ElemT, N>{static_cast<ElemT*>(::operator new(MIN_CAPACITY * sizeof(ElemT)))},
    size_(0), capacity_(MIN_CAPACITY) {
  }

  DynamicStorage(const size_t size) : IStorage<ElemT, N>{static_cast<ElemT*>(::operator new(size * sizeof(ElemT)))},
    size_(size), capacity_(size) {
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

  ~DynamicStorage() override {
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
    std::swap(*this, other);
    return *this;
  }

  [[nodiscard]] size_t Size() const override {
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
    } else {
      ElemT* old_buffer = buffer_;
      buffer_ = SafeMove(old_buffer, new_size, size_);
      DefaultConstruct(buffer_, size_, new_size);
      DestructAndDelete(old_buffer, size_);
      capacity_ = new_size;
    }

    size_ = new_size;
  }

  void PushBack(ElemT new_elem) {
    assert(size_ <= capacity_);

    if (size_ == capacity_) {
      ElemT* old_buffer = buffer_;

      const size_t new_capacity = 2 * size_ + 1;

      buffer_ = SafeMove(old_buffer, new_capacity, size_);
      DestructAndDelete(old_buffer, size_);
      capacity_ = new_capacity;
    }

    new (buffer_ + size_) ElemT(std::move(new_elem));
    ++size_;
  }

  void PopBack() {
    if (size_ == 0) {
      throw std::runtime_error(BAD_POP_MSG_);
    }

    Destruct(&buffer_[size_ - 1]);
    --size_;
  }

  ElemT& operator[](const size_t index) {
    if (index >= size_) {
      throw std::range_error(BAD_INDEX_MSG_);
    }

    return buffer_[index];
  }

  [[nodiscard]] const ElemT& operator[](const size_t index) const {
    if (index >= size_) {
      throw std::range_error(BAD_INDEX_MSG_);
    }

    return buffer_[index];
  }

 public:
  static const size_t MIN_CAPACITY = 8;

  static const char* const BAD_POP_MSG_;
  static const char* const BAD_INDEX_MSG_;

  size_t size_{0};
  size_t capacity_{0};

};

template<typename ElemT, size_t N>
const char* const DynamicStorage<ElemT, N>::BAD_POP_MSG_ = "attempt to remove the last element of an empty vector";

template<typename ElemT, size_t N>
const char* const DynamicStorage<ElemT, N>::BAD_INDEX_MSG_ = "trying to access on vector with invalid index";


#endif /* dynamic_storage.hpp */

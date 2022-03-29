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
  }

  DynamicStorage(const DynamicStorage& other) {
    buffer_ = SafeCopy(other.buffer_, other.size_, other.size_);
    size_ = capacity_ = other.size_;
  }

  DynamicStorage(DynamicStorage&& other) {
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    std::swap(this->buffer_, other.buffer_);
  }

  ~DynamicStorage() override {
    if (buffer_ != nullptr) {
      Destruct(buffer_, size_);
      ::operator delete(buffer_);
    }
    size_ = 0;
    capacity_ = 0;
  }

  DynamicStorage& operator=(const DynamicStorage& other) {
    DynamicStorage tmp(other);
    std::swap(*this, tmp);
    return *this;
  }

  DynamicStorage& operator=(DynamicStorage&& other) {
    DynamicStorage tmp(other);
    std::swap(*this, other);
    return *this;
  }

  size_t Size() const override {
    return size_;
  }

  void Resize(const size_t new_size) {
    if (size_ == new_size) {
      return;
    }

    if (new_size <= capacity_) {
      if (new_size < size_) {
        Destruct(buffer_, new_size, size_ - new_size);
      } else {
        while (size_ < new_size) {
          DefaultConstruct(buffer_ + size_);
          ++size_;
        }
      }
    } else {
      ElemT* old_buffer = buffer_;
      buffer_ = SafeCopy(old_buffer, new_size, std::min(new_size, size_));
      Destruct(old_buffer, size_);
      ::operator delete(old_buffer);
    }

    size_ = capacity_ = new_size;
  }

  void PushBack(ElemT new_elem) {
    assert(size_ <= capacity_);

    if (size_ == capacity_) {
      ElemT* old_buffer = buffer_;

      const size_t new_capacity = 2 * size_ + 1;

      buffer_ = SafeCopy(old_buffer, new_capacity, size_);
      ::operator delete(old_buffer);

      capacity_ = new_capacity;
    }

    new (buffer_ + size_) ElemT(std::move(new_elem));
    ++size_;
  }

  void PopBack() {
    if (size_ == 0) {
      throw std::runtime_error(BAD_POP_MSG);
    }

    Destruct(&buffer_[size_ - 1]);
    --size_;
  }

 private:
  static const size_t MIN_CAPACITY = 8;
  static const char* const BAD_POP_MSG;

  size_t size_{0};
  size_t capacity_{0};

};

template<typename ElemT, size_t N>
const char* const DynamicStorage<ElemT, N>::BAD_POP_MSG = "attempt to remove the last element of an empty vector";


#endif /* dynamic_storage.hpp */

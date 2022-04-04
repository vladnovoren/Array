#ifndef STATIC_STORAGE_HPP
#define STATIC_STORAGE_HPP

#include <cstddef>
#include <cassert>
#include <utility>
#include <cstdint>
#include <new>
#include "object_helpers.hpp"

template<
  typename ElemT,
  size_t MaxSize
>
class StaticStorage {
 public:
  StaticStorage() {
  }

  StaticStorage(const size_t size) {
    while (size_ < size) {
      DefaultConstruct(buffer_ + size_);
      ++size_;
    }
  }

  StaticStorage(const StaticStorage& other_copy) {
    while (size_ < other_copy.size_) {
      Construct(buffer_ + size_, other_copy.At(size_));
      ++size_;
    }
  }

  StaticStorage(StaticStorage&& other_move) {
    while (size_ < other_move.size_) {
      Construct(buffer_ + size_, std::move(other_move.At(size_)));
      ++size_;
    }
    other_move.buffer_ = nullptr;
    other_move.size_ = 0;
  }

  ~StaticStorage() {
    for (size_t i = 0; i < size_; ++i) {
      Destruct(buffer_ + i);
    }
  }

  StaticStorage& operator=(StaticStorage& other_copy) {
    if (other_copy == *this) {
      return *this;
    }

    StaticStorage tmp(other_copy);
    std::swap(*this, other_copy);
    return *this;
  }

  StaticStorage& operator=(StaticStorage&& other_move) {
    if (other_move == *this) {
      return *this;
    }


    for (size_t i = 0; i < other_move.size_; ++i) {
      Construct(buffer_ + i, std::move(other_move.At(i)));
      size_ = std::max(size_, i);
    }

  }

  [[nodiscard]] inline size_t Size() const {
    return size_;
  }

  [[nodiscard]] inline ElemT* Buffer() {
    return buffer_;
  }

  [[nodiscard]] inline const ElemT* Buffer() const {
    return buffer_;
  }

  [[nodiscard]] inline ElemT& At(const size_t index) {
    return buffer_[index];
  }

  [[nodiscard]] inline const ElemT& At(const size_t index) const {
    return buffer_[index];
  }

  void Resize(const size_t new_size) {
    if (new_size == size_) {
      return;
    }

    if (new_size > MaxSize) {
      throw std::out_of_range(BAD_STATIC_OVRFLW);
    }

    if (new_size < size_) {
      Destruct(buffer_, size_, new_size);
    } else {
      while (size_ < new_size) {
        Construct(buffer_, size_);
        ++size_;
      }
    }
  }

 private:
  uint8_t raw_buffer_[MaxSize * sizeof(ElemT)];
  ElemT* buffer_ = reinterpret_cast<ElemT*>(raw_buffer_);

  size_t size_{0};
  
};

#endif /* static_storage.hpp */

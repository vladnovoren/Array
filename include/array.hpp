#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "error_msgs.hpp"
#include <cstddef>
#include <stdexcept>
#include "dynamic_storage.hpp"
#include "static_storage.hpp"

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

  template<typename ArgT>
  Array(const size_t size, ArgT&& arg) : storage_(size, std::forward<ArgT>(arg)) {
  }

  Array(const Array& other) = default;

  Array(Array&& other) = default;

  ~Array() {
  }

  Array& operator=(const Array& other_copy) {
    Array tmp(other_copy);
    std::swap(*this, tmp);
    return *this;
  }

  Array& operator=(Array&& other_move) {
    Array tmp(other_move);
    std::swap(storage_, other_move.storage_);
    return *this;
  }

  [[nodiscard]] ElemT& operator[](const size_t index) {
    if (index >= Size()) {
      throw std::runtime_error(BAD_INDEX_MSG);
    }

    return storage_.Buffer()[index];
  }

  [[nodiscard]] const ElemT& operator[](const size_t index) const {
    if (index >= Size()) {
      throw std::runtime_error(BAD_INDEX_MSG);
    }

    return storage_.Buffer()[index];
  }

  [[nodiscard]] inline size_t Size() const {
    return storage_.Size();
  }

  [[nodiscard]] inline ElemT& Back() {
    if (storage_.Size() == 0) {
      throw std::logic_error(BAD_BACK_MSG);
    }

    return storage_.At(storage_.Size() - 1);
  }

  [[nodiscard]] inline const ElemT& Back() const {
    return static_cast<Array*>(this)->Back();
  }

  void Resize(const size_t new_size) {
    storage_.Resize(new_size);
  }

  template<typename OtherT>
  void PushBack(OtherT&& new_elem) {
    storage_.Resize(storage_.Size() + 1);
    storage_.At(storage_.Size() - 1) = std::forward<OtherT>(new_elem);
  }

  template<typename... ArgsT>
  void EmplaceBack(ArgsT&&... args) {
    storage_.Resize(storage_.Size() + 1);
    Construct(&storage_.At(storage_.Size() - 1), std::forward<ArgsT>(args)...);
  }

  void PopBack() {
    if (storage_.Size() == 0) {
      throw std::range_error(BAD_POP_MSG);
    }

    storage_.Resize(storage_.Size() - 1);
  }

 private:
  Storage<ElemT, N> storage_;

};

#endif /* array.hpp */

#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "error_msgs.hpp"
#include <cstddef>
#include <stdexcept>
#include "dynamic_storage.hpp"
#include "static_storage.hpp"

template<
  typename ElemT,
  template<typename StorageT, size_t StorageSize> class Storage = DynamicStorage,
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

  Array(const Array& other_copy) = default;

  Array(Array&& other_move) = default;

  ~Array() {
  }

  Array& operator=(const Array& other_copy) {
    if (this == &other_copy) {
      return *this;
    }

    Array tmp(other_copy);
    std::swap(*this, tmp);
    return *this;
  }

  Array& operator=(Array&& other_move) {
    if (this == &other_move) {
      return *this;
    }

    Array tmp(std::move(other_move));
    std::swap(storage_, tmp.storage_);
    return *this;
  }

  [[nodiscard]] inline ElemT& At(const size_t index) noexcept {
    return storage_.At(index);
  }

  [[nodiscard]] inline const ElemT& At(const size_t index) const noexcept {
    return storage_.At(index);
  }

  [[nodiscard]] ElemT& operator[](const size_t index) {
    if (index >= Size()) {
      throw std::out_of_range(BAD_INDEX_MSG);
    }

    return storage_.At(index);
  }

  [[nodiscard]] const ElemT& operator[](const size_t index) const {
    return const_cast<Array*>(this)->operator[](index);
  }

  [[nodiscard]] inline size_t Size() const noexcept {
    return storage_.Size();
  }

  [[nodiscard]] inline ElemT& Front() {
    if (storage_.Size() == 0) {
      throw std::logic_error(BAD_FRONT_MSG);
    }

    return storage_.At(0);
  }

  [[nodiscard]] inline const ElemT& Front() const {
    return const_cast<Array*>(this)->Front();
  }

  [[nodiscard]] inline ElemT& Back() {
    if (storage_.Size() == 0) {
      throw std::logic_error(BAD_BACK_MSG);
    }

    return storage_.At(storage_.Size() - 1);
  }

  [[nodiscard]] inline const ElemT& Back() const {
    return const_cast<Array*>(this)->Back();
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

  void Shrink() {
    storage_.Shrink();
  }

 private:
  Storage<ElemT, N> storage_;

};

template<
  template<typename StorageT, size_t StorageSize> class Storage,
  size_t N
>
class Array<bool, Storage, N> {
  class BoolProxy;

 public:
  Array() {
  }

  Array(const size_t size) : storage_(CalcSize(size)), size_{size} {
  }

  Array(const Array& other_copy) = default;

  Array(Array&& other_move) = default;

  ~Array() {
  }

  Array& operator=(const Array& other_copy) {
    if (this == &other_copy) {
      return *this;
    }

    Array tmp(other_copy);
    std::swap(*this, tmp);
    return *this;
  }

  Array& operator=(Array&& other_move) {
    if (this == &other_move) {
      return *this;
    }

    Array tmp(std::move(other_move));
    SwapFields(other_move);
    return *this;
  }

  [[nodiscard]] inline BoolProxy At(const size_t index) noexcept {
    return BoolProxy(storage_.At((index >> OFFSET_)), index % BITS_CNT_);
  }

  [[nodiscard]] inline const BoolProxy At(const size_t index) const noexcept {
    return const_cast<Array*>(this)->At(index);
  }

  [[nodiscard]] inline BoolProxy operator[](const size_t index) {
    if (index >= size_) {
      throw std::out_of_range(BAD_INDEX_MSG);
    }

    return At(index);
  }

  [[nodiscard]] inline const BoolProxy operator[](const size_t index) const {
    return const_cast<Array*>(this)->operator[](index);
  }

  [[nodiscard]] inline size_t Size() const noexcept {
    return size_;
  }

  [[nodiscard]] inline BoolProxy Front() {
    if (size_ == 0) {
      throw std::logic_error(BAD_FRONT_MSG);
    }

    return storage_.At(0);
  }

  [[nodiscard]] inline const BoolProxy Front() const {
    return const_cast<Array*>(this)->Front();
  }

  void Resize(const size_t new_size) {
    storage_.Resize(CalcSize(new_size));
  }

  void PushBack(bool value) {
    storage_.Resize(CalcSize(size_ + 1));
    size_++;

    storage_.At(size_ - 1) = value;
  }

  void EmplaceBack(bool value) {
    PushBack(value);
  }

  void PopBack() {
    if (size_ == 0) {
      throw std::logic_error(BAD_POP_MSG);
    }

    storage_.Resize(size_ - 1);
  }

  void Shrink() {
    storage_.Shrink();
  }

 private:
  void SwapFields(Array& other) {
    std::swap(storage_, other.storage_);
    std::swap(size_, other.size_);
  }

  static constexpr inline size_t CalcSize(const size_t size) {
    return ((size + BITS_CNT_ - 1) >> OFFSET_);
  }

 private:
  class BoolProxy {
   public:
    BoolProxy(uint8_t& byte, uint8_t bit) : byte_(byte), bit_(bit) {
      assert(bit < BITS_CNT_);
    }

    BoolProxy(const BoolProxy& other_copy) = delete;

    BoolProxy(BoolProxy&& other_move) = delete;

    inline bool GetValue() const noexcept {
      return (byte_ >> bit_) & 0x1;
    }

    void SetValue(bool value) {
      byte_ &= ~(1 << bit_);
      if (value) {
        byte_ |= (value << bit_);
      }
    }

    operator bool() const noexcept {
      return GetValue();
    }

    BoolProxy& operator=(bool value) noexcept {
      SetValue(value);
      return *this;
    }

    BoolProxy& operator=(const BoolProxy& other_copy) noexcept {
      SetValue(other_copy.GetValue());
      return *this;
    }

    BoolProxy& operator=(BoolProxy&& other_move) noexcept {
      SetValue(other_move.GetValue());
      return *this;
    }

   private:
    uint8_t& byte_;
    const uint8_t bit_;

  };

 private:
  static const size_t BITS_CNT_ = 8;
  static const size_t OFFSET_   = 3;

  Storage<uint8_t, CalcSize(N)> storage_;
  size_t size_{0};

};

#endif /* array.hpp */

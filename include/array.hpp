#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <cstddef>
#include <stdexcept>
#include <iterator>
#include <initializer_list>

#include "error_msgs.hpp"
#include "dynamic_storage.hpp"
#include "static_storage.hpp"
#include "chunked_storage.hpp"

template<
  typename ElemT,
  template<typename StorageT, size_t StorageSize> class Storage,
  size_t N
>
class Array;

template<typename Array, typename ElemT>
class BaseArrayIterator {
 public:
  static constexpr const bool is_const = std::is_same_v<const typename Array::value_type, ElemT>;

  friend typename std::conditional_t<is_const,
    BaseArrayIterator<std::remove_const_t<Array>, std::remove_const_t<ElemT>>,
    BaseArrayIterator<const Array, const ElemT>
  >;

 public:
  using iterator_category = std::random_access_iterator_tag;

  using value_type      = typename Array::value_type;
  using pointer         = std::conditional_t<is_const,
                                             typename Array::const_pointer,
                                             typename Array::pointer>;
  using reference       = std::conditional_t<is_const,
                                             typename Array::const_reference,
                                             typename Array::reference>;
  using difference_type = std::ptrdiff_t;

 public:
  BaseArrayIterator(Array* array, const size_t index) :
    array_{array}, index_{index} {
  }

  template<typename OtherArray, typename OtherElemT>
  BaseArrayIterator(const BaseArrayIterator<OtherArray, OtherElemT>& other_copy) :
    array_{other_copy.array_}, index_{other_copy.index_} {
    ValidRhs(other_copy);
  }

  template<typename OtherArray, typename OtherElemT>
  BaseArrayIterator(BaseArrayIterator<OtherArray, OtherElemT>&& other_move) {
    ValidRhs(other_move);

    std::swap(this->array_, other_move.array_);
    std::swap(this->index_, other_move.index_);
  }

  template<typename OtherArray, typename OtherElemT>
  BaseArrayIterator& operator=(const BaseArrayIterator<OtherArray, OtherElemT>& other_copy) {
    ValidRhs(other_copy);

    if ((void*)this == (void*)&other_copy) {
      return *this;
    }

    BaseArrayIterator tmp(other_copy);
    std::swap(*this, tmp);
    return *this;
  }

  template<typename OtherArray, typename OtherElemT>
  BaseArrayIterator& operator=(BaseArrayIterator<OtherArray, OtherElemT>&& other_move) {
    ValidRhs(other_move);

    if ((void*)this == (void*)&other_move) {
      return *this;
    }

    std::swap(this->array_, other_move.array_);
    std::swap(this->index_, other_move.index_);
    return *this;
  }

  BaseArrayIterator& operator+=(const difference_type diff) {
    assert(array_ != nullptr);

    CheckValid(index_ + diff);

    index_ += diff;
    return *this;
  }

  BaseArrayIterator& operator-=(const difference_type diff) {
    return this->operator+=(-diff);
  }

  BaseArrayIterator& operator++() {
    return this->operator+=(1);
  }

  BaseArrayIterator& operator--() {
    return this->operator-=(1);
  }

  BaseArrayIterator operator++(int) {
    BaseArrayIterator old(*this);
    this->operator++();
    return old;
  }

  BaseArrayIterator operator--(int) {
    BaseArrayIterator old(*this);
    this->operator--();
    return old;
  }

  reference operator[](const size_t offset) {
    CheckOverflow(index_ + offset);
    return array_->At(index_ + offset);
  }

  template<typename OtherArray, typename OtherElemT>
  difference_type operator-(const BaseArrayIterator<OtherArray, OtherElemT>& rhs) {
    CheckSameArrays(rhs);

    return index_ - rhs.index_;
  }

  reference operator*() const {
    CheckOverflow(index_);

    return array_->At(index_);
  }

  pointer operator->() const {
    CheckOverflow(index_);

    return &array_->At(index_);
  }

  template<typename OtherArray, typename OtherElemT>
  bool operator==(const BaseArrayIterator<OtherArray, OtherElemT>& rhs) const {
    CheckSameArrays(rhs);

    return index_ == rhs.index_;
  }

  template<typename OtherArray, typename OtherElemT>
  bool operator!=(const BaseArrayIterator<OtherArray, OtherElemT>& rhs) const {
    return !(*this == rhs);
  }

  template<typename OtherArray, typename OtherElemT>
  bool operator<(const BaseArrayIterator<OtherArray, OtherElemT>& rhs) const {
    CheckSameArrays(rhs);

    return index_ < rhs.index_;
  }

  template<typename OtherArray, typename OtherElemT>
  bool operator>(const BaseArrayIterator<OtherArray, OtherElemT>& rhs) const {
    return rhs < *this;
  }

  template<typename OtherArray, typename OtherElemT>
  bool operator<=(const BaseArrayIterator<OtherArray, OtherElemT>& rhs) const {
    return !(*this > rhs);
  }

  template<typename OtherArray, typename OtherElemT>
  bool operator>=(const BaseArrayIterator<OtherArray, OtherElemT>& rhs) const {
    return !(*this < rhs);
  }

 private:
  static constexpr const char* const OUT_OF_RANGE_MSG_     = "iterator is out of range";
  static constexpr const char* const INVALID_ITERATOR_MSG_ = "iterator is invalid";
  static constexpr const char* const DIFFERENT_ARRAYS_MSG_ = "iterators of different arrays";

 private:
  void CheckValid(const size_t index) const {
    if (index != -1ULL && index > array_->Size()) {
      throw std::logic_error(INVALID_ITERATOR_MSG_);
    }
  }

  void CheckOverflow(const size_t index) const {
    if (index >= array_->Size()) {
      throw std::out_of_range(OUT_OF_RANGE_MSG_);
    }
  }

  void CheckSameArrays(const BaseArrayIterator& other) const {
    if (array_ != other.array_) {
      throw std::logic_error(DIFFERENT_ARRAYS_MSG_);
    }
  }

 private:
  Array* array_{nullptr};
  size_t index_{0};

 private:
  template<typename OtherIterator>
  static constexpr void ValidRhs(const OtherIterator&) {
    static_assert(is_const == OtherIterator::is_const ||
                  (is_const && !OtherIterator::is_const));
  }

};

template<typename Array, typename ElemT>
class BaseArrayIterator;

template<typename Array>
using ArrayIterator = BaseArrayIterator<Array, typename Array::value_type>;

template<typename Array>
using ConstArrayIterator = BaseArrayIterator<const Array, const typename Array::value_type>;

template<
  typename ElemT,
  template<typename StorageT, size_t StorageSize> class Storage = DynamicStorage,
  size_t N = 0
>
class Array {
 public:
  using value_type = ElemT;

  using pointer = ElemT*;
  using const_pointer = const ElemT*;

  using reference = ElemT&;
  using const_reference = const ElemT&;

  using difference_type = std::ptrdiff_t;

 public:
  Array() {
  }

  Array(const std::initializer_list<ElemT>& init_list) {
    for (const ElemT& value : init_list) {
      new (storage_.ReserveBack()) ElemT(value);
    }
  }

  explicit Array(const size_t size) : storage_(size) {
  }

  Array(const size_t size, const ElemT& value) : storage_(size, value) {
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

    std::swap(storage_, other_move.storage_);
    return *this;
  }

  ArrayIterator<Array> begin() {
    return ArrayIterator<Array>(this, 0);
  }

  ArrayIterator<Array> end() {
    return ArrayIterator<Array>(this, Size());
  }

  ConstArrayIterator<Array> cbegin() const {
    return ConstArrayIterator<Array>(this, 0);
  }

  ConstArrayIterator<Array> cend() const {
    return ConstArrayIterator<Array>(this, Size());
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

  template<typename... ArgsT>
  void EmplaceBack(ArgsT&&... args) {
    storage_.ReserveBack();
    try {
      ConstructOne(&storage_.At(storage_.Size() - 1), std::forward<ArgsT>(args)...);
    } catch (...) {
      storage_.RollBackReservedBack();
      throw;
    }
  }

  template<typename OtherT>
  void PushBack(OtherT&& new_elem) {
    EmplaceBack(std::forward<OtherT>(new_elem));
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

  Array(const std::initializer_list<bool>& init_list) {
    for (bool value : init_list) {
      EmplaceBack(value);
    }
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
    return BoolProxy(storage_.At(index >> OFFSET_), index % BITS_CNT_);
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

    return At(0);
  }

  [[nodiscard]] inline const BoolProxy Front() const {
    return const_cast<Array*>(this)->Front();
  }

  void Resize(const size_t new_size) {
    storage_.Resize(CalcSize(new_size));
  }

  void EmplaceBack(bool value) {
    storage_.Resize(CalcSize(size_ + 1));
    size_++;

    At(size_ - 1) = value;
  }

  void PushBack(bool value) {
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

    inline bool GetValue() const noexcept {
      return (byte_ >> bit_) & 0x1;
    }

    void SetValue(bool value) {
      byte_ &= ~(1 << bit_);
      if (value) {
        byte_ |= (1 << bit_);
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

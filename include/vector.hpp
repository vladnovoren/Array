#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cstddef>
#include <stdexcept>
#include <iterator>
#include <initializer_list>

#include "error_msgs.hpp"
#include "dynamic_storage.hpp"
#include "static_storage.hpp"
#include "chunked_storage.hpp"

// BaseVectorIterator

template<
  typename ElemT,
  template<typename StorageT, size_t StorageSize> class Storage,
  size_t N
>
class Vector;

template<typename Vector, typename ElemT>
class BaseVectorIterator {
 public:
  static constexpr const bool is_const = std::is_same_v<const typename Vector::value_type, ElemT>;

  friend typename std::conditional_t<is_const,
    BaseVectorIterator<std::remove_const_t<Vector>, std::remove_const_t<ElemT>>,
    BaseVectorIterator<const Vector, const ElemT>
  >;

 public:
  using iterator_category = typename Vector::iterator_category;

  using value_type      = typename Vector::value_type;
  using pointer         = std::conditional_t<is_const,
                                             typename Vector::const_pointer,
                                             typename Vector::pointer>;
  using reference       = std::conditional_t<is_const,
                                             typename Vector::const_reference,
                                             typename Vector::reference>;
  using difference_type = std::ptrdiff_t;

 public:
  BaseVectorIterator(Vector* vector, const size_t index) :
    vector_{vector}, index_{index} {
  }

  template<typename OtherVector, typename OtherElemT>
  BaseVectorIterator(const BaseVectorIterator<OtherVector, OtherElemT>& other_copy) :
    vector_{other_copy.vector_}, index_{other_copy.index_} {
    ValidRhs(other_copy);
  }

  template<typename OtherVector, typename OtherElemT>
  BaseVectorIterator(BaseVectorIterator<OtherVector, OtherElemT>&& other_move) {
    ValidRhs(other_move);

    std::swap(this->vector_, other_move.vector_);
    std::swap(this->index_, other_move.index_);
  }

  template<typename OtherVector, typename OtherElemT>
  BaseVectorIterator& operator=(const BaseVectorIterator<OtherVector, OtherElemT>& other_copy) {
    ValidRhs(other_copy);

    if ((void*)this == (void*)&other_copy) {
      return *this;
    }

    BaseVectorIterator tmp(other_copy);
    std::swap(*this, tmp);
    return *this;
  }

  template<typename OtherVector, typename OtherElemT>
  BaseVectorIterator& operator=(BaseVectorIterator<OtherVector, OtherElemT>&& other_move) {
    ValidRhs(other_move);

    if ((void*)this == (void*)&other_move) {
      return *this;
    }

    std::swap(this->vector_, other_move.vector_);
    std::swap(this->index_, other_move.index_);
    return *this;
  }

  inline BaseVectorIterator& operator+=(const difference_type diff) {
    assert(vector_ != nullptr);

    CheckValid(index_ + diff);

    index_ += diff;
    return *this;
  }

  inline BaseVectorIterator& operator-=(const difference_type diff) {
    return this->operator+=(-diff);
  }

  inline BaseVectorIterator& operator++() {
    return this->operator+=(1);
  }

  inline BaseVectorIterator& operator--() {
    return this->operator-=(1);
  }

  inline BaseVectorIterator operator++(int) {
    BaseVectorIterator old(*this);
    this->operator++();
    return old;
  }

  inline BaseVectorIterator operator--(int) {
    BaseVectorIterator old(*this);
    this->operator--();
    return old;
  }

  inline reference operator[](const size_t offset) {
    CheckOverflow(index_ + offset);
    return vector_->At(index_ + offset);
  }

  inline BaseVectorIterator operator+(difference_type diff) const {
    CheckValid(index_ + diff);
    return BaseVectorIterator<Vector, ElemT>(vector_, index_ + diff);
  }

  inline BaseVectorIterator operator-(difference_type diff) const {
    return operator+(-diff);
  }

  template<typename OtherVector, typename OtherElemT>
  inline difference_type operator-(const BaseVectorIterator<OtherVector, OtherElemT>& rhs) const {
    CheckSameVectors(rhs);

    return index_ - rhs.index_;
  }

  inline reference operator*() const {
    CheckOverflow(index_);

    return vector_->At(index_);
  }

  inline pointer operator->() const {
    CheckOverflow(index_);

    return &vector_->At(index_);
  }

  template<typename OtherVector, typename OtherElemT>
  inline bool operator==(const BaseVectorIterator<OtherVector, OtherElemT>& rhs) const {
    CheckSameVectors(rhs);

    return index_ == rhs.index_;
  }

  template<typename OtherVector, typename OtherElemT>
  inline bool operator!=(const BaseVectorIterator<OtherVector, OtherElemT>& rhs) const {
    return !(*this == rhs);
  }

  template<typename OtherVector, typename OtherElemT>
  inline bool operator<(const BaseVectorIterator<OtherVector, OtherElemT>& rhs) const {
    CheckSameVectors(rhs);

    return index_ < rhs.index_;
  }

  template<typename OtherVector, typename OtherElemT>
  inline bool operator>(const BaseVectorIterator<OtherVector, OtherElemT>& rhs) const {
    return rhs < *this;
  }

  template<typename OtherVector, typename OtherElemT>
  inline bool operator<=(const BaseVectorIterator<OtherVector, OtherElemT>& rhs) const {
    return !(*this > rhs);
  }

  template<typename OtherVector, typename OtherElemT>
  inline bool operator>=(const BaseVectorIterator<OtherVector, OtherElemT>& rhs) const {
    return !(*this < rhs);
  }

 private:
  static constexpr const char* const OUT_OF_RANGE_MSG_     = "iterator is out of range";
  static constexpr const char* const INVALID_ITERATOR_MSG_ = "iterator is invalid";
  static constexpr const char* const DIFFERENT_VECTORS_MSG_ = "iterators of different vectors";

 private:
  inline void CheckValid(const size_t index) const {
    if (index != -1ULL && index > vector_->Size()) {
      throw std::logic_error(INVALID_ITERATOR_MSG_);
    }
  }

  inline void CheckOverflow(const size_t index) const {
    if (index >= vector_->Size()) {
      throw std::out_of_range(OUT_OF_RANGE_MSG_);
    }
  }

  inline void CheckSameVectors(const BaseVectorIterator& other) const {
    if (vector_ != other.vector_) {
      throw std::logic_error(DIFFERENT_VECTORS_MSG_);
    }
  }

 private:
  Vector* vector_{nullptr};
  size_t index_{0};

 private:
  template<typename OtherIterator>
  static constexpr void ValidRhs(const OtherIterator&) {
    static_assert(is_const == OtherIterator::is_const ||
                  (is_const && !OtherIterator::is_const));
  }

};

// VectorIterator aliases

template<typename Vector>
using VectorIterator = BaseVectorIterator<Vector, typename Vector::value_type>;

template<typename Vector>
using ConstVectorIterator = BaseVectorIterator<const Vector, const typename Vector::value_type>;


// Vector

template<
  typename ElemT,
  template<typename StorageT, size_t StorageSize> class Storage = DynamicStorage,
  size_t N = 0
>
class Vector {
 public:
  using value_type = ElemT;

  using pointer = ElemT*;
  using const_pointer = const ElemT*;

  using reference = ElemT&;
  using const_reference = const ElemT&;

  using difference_type = std::ptrdiff_t;

  using iterator_category = std::random_access_iterator_tag;

 public:
  Vector() = default;

  Vector(const std::initializer_list<ElemT>& init_list) {
    for (const ElemT& value : init_list) {
      new (storage_.ReserveBack()) ElemT(value);
    }
  }

  explicit Vector(const size_t size) : storage_(size) {
  }

  Vector(const size_t size, const ElemT& value) : storage_(size, value) {
  }

  Vector(const Vector& other_copy) = default;

  Vector(Vector&& other_move) = default;

  ~Vector() = default;

  Vector& operator=(const Vector& other_copy) {
    if (this == &other_copy) {
      return *this;
    }

    Vector tmp(other_copy);
    std::swap(*this, tmp);
    return *this;
  }

  Vector& operator=(Vector&& other_move) {
    if (this == &other_move) {
      return *this;
    }

    std::swap(storage_, other_move.storage_);
    return *this;
  }

  inline ConstVectorIterator<Vector> cbegin() const {
    return ConstVectorIterator<Vector>(this, 0);
  }

  inline ConstVectorIterator<Vector> cend() const {
    return ConstVectorIterator<Vector>(this, Size());
  }

  inline VectorIterator<Vector> begin() {
    return VectorIterator<Vector>(this, 0);
  }

  inline ConstVectorIterator<Vector> begin() const {
    return cbegin();
  }

  inline VectorIterator<Vector> end() {
    return VectorIterator<Vector>(this, Size());
  }

  inline ConstVectorIterator<Vector> end() const {
    return cend();
  }

  inline std::reverse_iterator<ConstVectorIterator<Vector>> crbegin() const {
    return std::make_reverse_iterator(ConstVectorIterator<Vector>(this, 0));
  }

  inline std::reverse_iterator<ConstVectorIterator<Vector>> crend() const {
    return std::make_reverse_iterator(ConstVectorIterator<Vector>(this, Size()));
  }

  inline std::reverse_iterator<VectorIterator<Vector>> rbegin() {
    return std::make_reverse_iterator(VectorIterator<Vector>(this, 0));
  }

  inline std::reverse_iterator<VectorIterator<Vector>> rend() {
    return std::make_reverse_iterator(VectorIterator<Vector>(this, Size()));
  }

  inline std::reverse_iterator<ConstVectorIterator<Vector>> rbegin() const {
    return crbegin();
  }

  inline std::reverse_iterator<ConstVectorIterator<Vector>> rend() const {
    return crend();
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
    return const_cast<Vector*>(this)->operator[](index);
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
    return const_cast<Vector*>(this)->Front();
  }

  [[nodiscard]] inline ElemT& Back() {
    if (storage_.Size() == 0) {
      throw std::logic_error(BAD_BACK_MSG);
    }

    return storage_.At(storage_.Size() - 1);
  }

  [[nodiscard]] inline const ElemT& Back() const {
    return const_cast<Vector*>(this)->Back();
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

class BoolProxy {
  public:
  BoolProxy(uint8_t& byte, uint8_t bit) : byte_(byte), bit_(bit) {
    assert(bit < BITS_CNT_);
  }

  BoolProxy(const BoolProxy& other_copy) : byte_(other_copy.byte_), bit_(other_copy.bit_) {}
  BoolProxy(BoolProxy&& other_move) : byte_(other_move.byte_), bit_(other_move.bit_) {}

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

  BoolProxy& operator=(BoolProxy&& other_move) noexcept {
    SetValue(other_move.GetValue());
    return *this;
  }

 private:
  uint8_t& byte_;
  const uint8_t bit_;

 public:
  static const size_t BITS_CNT_ = 8;
  static const size_t OFFSET_   = 3;

};


// bool sepcialization

template<
  template<typename StorageT, size_t StorageSize> class Storage,
  size_t N
>
class Vector<bool, Storage, N> {
 public:
  using value_type = bool;

  using pointer = bool*;
  using const_pointer = const bool*;

  using reference = BoolProxy;
  using const_reference = const BoolProxy;

  using difference_type = std::ptrdiff_t;

  using iterator_category = std::random_access_iterator_tag;

 public:
  Vector() = default;

  Vector(const std::initializer_list<bool>& init_list) {
    for (bool value : init_list) {
      EmplaceBack(value);
    }
  }

  explicit Vector(const size_t size) : storage_(CalcSize(size)), size_{size} {
  }

  Vector(const Vector& other_copy) = default;

  Vector(Vector&& other_move) = default;

  ~Vector() = default;

  inline ConstVectorIterator<Vector> cbegin() const {
    return ConstVectorIterator<Vector>(this, 0);
  }

  inline ConstVectorIterator<Vector> cend() const {
    return ConstVectorIterator<Vector>(this, Size());
  }

  inline VectorIterator<Vector> begin() {
    return VectorIterator<Vector>(this, 0);
  }

  inline ConstVectorIterator<Vector> begin() const {
    return cbegin();
  }

  inline VectorIterator<Vector> end() {
    return VectorIterator<Vector>(this, Size());
  }

  inline ConstVectorIterator<Vector> end() const {
    return cend();
  }

  inline std::reverse_iterator<ConstVectorIterator<Vector>> crbegin() const {
    return std::make_reverse_iterator(ConstVectorIterator<Vector>(this, 0));
  }

  inline std::reverse_iterator<ConstVectorIterator<Vector>> crend() const {
    return std::make_reverse_iterator(ConstVectorIterator<Vector>(this, Size()));
  }

  inline std::reverse_iterator<VectorIterator<Vector>> rbegin() {
    return std::make_reverse_iterator(VectorIterator<Vector>(this, 0));
  }

  inline std::reverse_iterator<VectorIterator<Vector>> rend() {
    return std::make_reverse_iterator(VectorIterator<Vector>(this, Size()));
  }

  inline std::reverse_iterator<ConstVectorIterator<Vector>> rbegin() const {
    return crbegin();
  }

  inline std::reverse_iterator<ConstVectorIterator<Vector>> rend() const {
    return crend();
  }

  Vector& operator=(const Vector& other_copy) {
    if (this == &other_copy) {
      return *this;
    }

    Vector tmp(other_copy);
    std::swap(*this, tmp);
    return *this;
  }

  Vector& operator=(Vector&& other_move) {
    if (this == &other_move) {
      return *this;
    }

    Vector tmp(std::move(other_move));
    SwapFields(other_move);
    return *this;
  }

  [[nodiscard]] inline BoolProxy At(const size_t index) noexcept {
    return BoolProxy(storage_.At(index >> OFFSET_), index % BITS_CNT_);
  }

  [[nodiscard]] inline const BoolProxy At(const size_t index) const noexcept {
    return const_cast<Vector*>(this)->At(index);
  }

  [[nodiscard]] inline BoolProxy operator[](const size_t index) {
    if (index >= size_) {
      throw std::out_of_range(BAD_INDEX_MSG);
    }

    return At(index);
  }

  [[nodiscard]] inline const BoolProxy operator[](const size_t index) const {
    return const_cast<Vector*>(this)->operator[](index);
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
    return const_cast<Vector*>(this)->Front();
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
  void SwapFields(Vector& other) {
    std::swap(storage_, other.storage_);
    std::swap(size_, other.size_);
  }

  static constexpr inline size_t CalcSize(const size_t size) {
    return ((size + BITS_CNT_ - 1) >> OFFSET_);
  }

 private:
  static const size_t BITS_CNT_ = BoolProxy::BITS_CNT_;
  static const size_t OFFSET_   = BoolProxy::OFFSET_;

  Storage<uint8_t, CalcSize(N)> storage_;
  size_t size_{0};

};

void swap(BoolProxy a, BoolProxy b) {
  BoolProxy tmp = a;
  a = b;
  b = tmp;
}

#endif /* vector.hpp */

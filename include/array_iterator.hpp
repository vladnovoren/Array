#ifndef ARRAY_ITERATOR_HPP
#define ARRAY_ITERATOR_HPP

#include "array.hpp"

template<typename Array, bool IsConst>
class ArrayIterator {
  friend ArrayIterator<Array, !IsConst>;

 public:
  using value_type = Array::value_type;
  using pointer = Array::pointer;
  using reference = Array::reference;
  using difference_type = std::ptrdiff_t;
  using 

 public:
  ArrayIterator(Array* array, const size_t index) : 
    array_{array}, index_{index} {
  }

  ArrayIterator& operator+=(const difference_type diff) {
    assert(array_ != nullptr);

    CheckOverflow(index_ + diff);

    index_ += diff;
    return *this;
  }

  ArrayIterator& operator-=(const difference_type diff) {
    return this->operator+=(-diff);
  }

  ArrayIterator& operator++() {
    return this->operator+=(1);
  }

  ArrayIterator& operator--() {
    return this->operator-=(1);
  }

  ArrayIterator operator++(int dummy) {
    ArrayIterator old(*this);
    this->operator++;
    return old;
  }

  ArrayIterator operator--(int dummy) {
    ArrayIterator old(*this);
    this->operator--;
    return old;
  }

  std::ptrdiff_t operator-(const ArrayIterator& rhs) {

  }

 private:
  static const char* const OUT_OF_RANGE_MSG_ = "iterator is out of range";

 private:
  void CheckOverflow(const size_t index) {
    if (index >= array_.Size()) {
      throw std::out_of_range(OUT_OF_RANGE_MSG_);
    }
  }

 private:
  Array* array_{nullptr};
  size_t index_{0};

};

#endif /* array_iterator.hpp */

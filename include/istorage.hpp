#ifndef ISTORAGE_HPP
#define ISTORAGE_HPP

#include <cstddef>

template<typename ElemT, size_t size>
class IStorage {
 public:
  IStorage() {
  }

  IStorage(ElemT* buffer) : buffer_(buffer) {
  }

  virtual ~IStorage() = 0;

  ElemT& operator[](const size_t index) {
    return buffer_[index];
  }

  const ElemT& operator[](const size_t index) const {
    return buffer_[index];
  }

  virtual size_t Size() const = 0;

 protected:
  ElemT* buffer_{nullptr};

};

template<typename ElemT, size_t size>
IStorage<ElemT, size>::~IStorage() {
}

#endif /* storage.hpp */

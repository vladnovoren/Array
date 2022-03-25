#ifndef ISTORAGE_HPP
#define ISTORAGE_HPP

#include <cstddef>

template<typename ElemT, size_t Size>
class IStorage {
 public:
  IStorage() {
  }

  IStorage(ElemT* data) : data_(data) {
  }

  virtual ~IStorage() = 0;

  ElemT& operator[](const size_t index) {
    return data_[index];
  }

  const ElemT& operator[](const size_t index) const {
    return data_[index];
  }

  virtual size_t Size() const = 0;

 private:
  ElemT* data_{nullptr};

};

template<typename ElemT, size_t Size>
IStorage<ElemT, Size>::~IStorage() {
}

#endif /* storage.hpp */

#ifndef OBJECT_HELPERS_HPP
#define OBJECT_HELPERS_HPP

#include <new>
#include <cassert>

template<typename ElemT>
void CopyConstruct(ElemT* elem, const ElemT& other) {
  assert(elem != nullptr);

  new (elem) ElemT(other);
}

template<typename ElemT>
void DefaultConstruct(ElemT* elem) {
  assert(elem != nullptr);

  new (elem) ElemT();
}

template<typename ElemT>
void DefaultConstruct(ElemT* buffer, const size_t first, const size_t last) {
  assert(buffer != nullptr);

  for (size_t i = first; i < last; ++i) {
    DefaultConstruct(buffer + i);
  }
}

template<typename ElemT>
void DefaultConstruct(ElemT* buffer, const size_t size) {
  DefaultConstruct(buffer, 0, size);
}

template<typename ElemT>
void Destruct(ElemT* elem) {
  assert(elem != nullptr);

  elem->~ElemT();
}

template<typename ElemT>
void Destruct(ElemT* buffer, const size_t first, const size_t last) {
  assert(buffer != nullptr);

  for (size_t i = first; i < last; ++i) {
    Destruct(buffer + i);
  }
}

template<typename ElemT>
void Destruct(ElemT* buffer, const size_t size) {
  Destruct(buffer, 0, size);
}

template<typename ElemT>
ElemT* SafeCopy(ElemT* src, const size_t dst_size, const size_t src_size) {
  assert(src_size <= dst_size);

  ElemT* dst = new ElemT[dst_size];
  try {
    for (size_t i = 0; i < src_size; ++i) {
      dst[i] = src[i];
    }
  } catch (...) {
    delete[] dst;
    throw;
  }
  return dst;
}

#endif /* object_helpers.hpp */

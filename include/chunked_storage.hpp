#ifndef CHUNKED_STORAGE_HPP
#define CHUNKED_STORAGE_HPP

#include "dynamic_storage.hpp"

template<typename ElemT, size_t N = 0>
class ChunkedStorage {
 public:
  ChunkedStorage() {
  }

  ChunkedStorage(const size_t size) : chunks_(CalcChunksCnt(size)) {
    for (size_t i = 0; i < chunks_.Size() - 1; ++i) {
      DefaultConstructChunk(&chunks_.At(i), CHUNK_SIZE_);
      size_ += CHUNK_SIZE_;
    }
    DestructChunk(&chunks_.At(chunks_.Size() - 1), size - size_);
    size_ = size;
  }

  ChunkedStorage(const size_t size, const ElemT& value) : chunk_(CalcChunksCnt(size)) {
    for (size_t i = 0; i < chunks_.Size() - 1; ++i) {
      ConstructChunk(&chunks_.At(i), CHUNK_SIZE_, value);
      size_ += CHUNK_SIZE_;
    }
    DestructChunk(&chunks_.At(chunks_.Size() - 1), size - size_);
    size_ = size;
  }

  ~ChunkedStorage() {
    for (size_t i = 0; i < chunks_.Size() - 1; ++i) {
      DestructChunk(&chunks_.At(i), CHUNK_SIZE_);
    }
  }

 private:
  size_t CalcChunksCnt(const size_t size) {
    return (sizeof(ElemT) * size + CHUNK_SIZE_ - 1) / CHUNK_SIZE_;
  }

  template<typename ...ArgsT>
  void Construct(const size_t to_construct, const ArgsT&... args) {
    for (size_t i = 0; i < chunks_.Size() - 1; ++i) {
      ConstructChunk(&chunks_.At(i), CHUNK_SIZE_, args...);
      size_ += CHUNK_SIZE_;
    }
    ConstructChunk(&chunks_.At(chunks_.Size() - 1), size - size_, args...);
    size_ = size;
  }

  template<typename ...ArgsT>
  void ConstructChunk(ElemT* chunk, const ArgsT&... args) {
    assert(chunk != nullptr);

    size_t constructed = 0;
    try {
      constructed = Construct(chunk, CHUNK_SIZE_, args...);
    } catch (...) {
      Destruct(chunk, constructed);
      throw;
    }
  }

  void DefaultConstructChunk(ElemT* chunk, const size_t to_construct) {
    assert(chunk != nullptr);

    size_t constructed = 0;
    try {
      constructed = DefaultConstruct(chunk, to_construct);
    } catch (...) {
      Destruct(chunk, constructed);
      throw;
    }
  }

  void DestructChunk(ElemT* chunk, const size_t to_destruct = CHUNK_SIZE_) {
    assert(chunk != nullptr);

    Destruct(chunk, to_destruct);
  }

 private:
  static const size_t MIN_CHUNK_SIZE_ = 1024;
  static const size_t CHUNK_SIZE_ = std::max(MIN_CHUNK_SIZE_, sizeof(ElemT) * 8);

 private:
  DynamicStorage<ElemT*> chunks_;

  size_t size_ = 0;

};

#endif /* chunked_storage.hpp */

#ifndef CHUNKED_STORAGE_HPP
#define CHUNKED_STORAGE_HPP

#include "dynamic_storage.hpp"

template<typename ElemT, size_t N = 0>
class ChunkedStorage {
 public:
  ChunkedStorage() {
  }

  ChunkedStorage(const size_t size) : chunks_(CalcChunksCnt(size), nullptr) {
  }

  ChunkedStorage(const size_t size, const ElemT& value) : size_{size}, value_(value) {
  }

  ChunkedStorage(const ChunkedStorage& other_copy) = default;

  ChunkedStorage(ChunkedStorage&& other_move) {
    std::swap(chunks_, other_move.chunks_);
    std::swap(size_, other_move.size_);
    std::swap(value_, other_move.value_);
  }

  ~ChunkedStorage() {
    if (chunks_.Size() != 0) {
      for (size_t i = 0; i < chunks_.Size() - 1; ++i) {
        DestructAndDeleteChunk(chunks_.At(i), FULL_CHUNK_SIZE_);
      }
      DestructAndDeleteChunk(chunks_.At(chunks_.Size() - 1), size_ % FULL_CHUNK_SIZE_);
    }
  }

  ChunkedStorage& operator=(const ChunkedStorage& other_copy) {
    if (this == &other_copy) {
      return *this;
    }

    ChunkedStorage tmp(other_copy);
    std::swap(*this, tmp);
    return *this;
  }

  ChunkedStorage& operator=(ChunkedStorage&& other_move) {
    if (this == &other_move) {
      return *this;
    }

    std::swap(chunks_, other_move.chunks_);
    std::swap(size_, other_move.size_);
    std::swap(value_, other_move.value_);
    return *this;
  }

  [[nodiscard]] inline size_t Size() const {
    return size_;
  }

  [[nodiscard]] inline ElemT& At(const size_t index) {
    size_t chunk_num = GetChunkNum(index);
    size_t chunk_size = GetChunkSize(chunk_num);
    ElemT*& chunk = chunks_.At(chunk_num);
    if (chunk == nullptr) {
      MakeChunkReady(chunk, chunk_size);
    }
    return chunk[index % FULL_CHUNK_SIZE_];
  }

  [[nodiscard]] inline const ElemT& At(const size_t index) const {
    return const_cast<ChunkedStorage*>(this)->At(index);
  }

  void Resize(const size_t new_size) {
    if (size_ == new_size) {
      return;
    }

    if (new_size == 0) {
      if (chunks_.Size() != 0) {
        for (size_t i = 0; i < chunks_.Size() - 1; ++i) {
          Destruct(chunks_.At(i), FULL_CHUNK_SIZE_);
        }
        Destruct(chunks_.At(chunks_.Size() - 1), size_ % FULL_CHUNK_SIZE_);
      }
      return;
    }

    if (new_size < size_) {
      size_t start_chunk_num = GetChunkNum(new_size);
      Destruct(chunks_.At(start_chunk_num), new_size % FULL_CHUNK_SIZE_);
      for (size_t i = start_chunk_num + 1; i < chunks_.Size() - 1; ++i) {
        Destruct(chunks_.At(i), FULL_CHUNK_SIZE_);
      }
      Destruct(chunks_.At(chunks_.Size() - 1), size_ % FULL_CHUNK_SIZE_);
    } else {
      size_t new_chunks_cnt = GetChunkNum(new_size);
      chunks_.Resize(new_chunks_cnt);
    }

    size_ = new_size;
  }

  ElemT* ReserveBack() {
    size_t chunk_num = GetChunkNum(size_ + 1);

    assert(chunk_num <= chunks_.Size());
    if (chunk_num == chunks_.Size()) {
      chunks_.Resize(chunk_num + 1);
    }

    ElemT*& chunk = chunks_.At(chunk_num);

    if (chunk == nullptr) {
      MakeChunkReady(chunk, 0);
    }
    ++size_;

    return &chunk[(size_ - 1) % FULL_CHUNK_SIZE_];
  }

  void RollBackReservedBack() {
    --size_;
  }

  void Shrink() {
    for (size_t i = GetChunkNum(size_) + 1; i < chunks_.Size(); ++i) {
      ::operator delete(chunks_.At(i));
      chunks_.At(i) = nullptr;
    }
  }

 private:
  bool IsChunkReady(const size_t chunk_index) {
    return chunks_.At(chunk_index) != nullptr;
  }

  size_t CalcChunksCnt(const size_t size) {
    return (sizeof(ElemT) * size + CHUNK_CAP_ - 1) / CHUNK_CAP_;
  }

  size_t GetChunkNum(const size_t elem_index) {
    return elem_index / FULL_CHUNK_SIZE_;
  }

  size_t GetChunkSize(const size_t chunk_num) {
    return (chunk_num == chunks_.Size() - 1 || chunks_.Size() == 0) ? size_ % FULL_CHUNK_SIZE_ : FULL_CHUNK_SIZE_;
  }

  void DestructChunk(ElemT*& chunk, const size_t chunk_size) {
    if (chunk == nullptr) {
      return;
    }

    Destruct(chunk, chunk_size);
    chunk = nullptr;
  }

  void DestructAndDeleteChunk(ElemT*& chunk, const size_t chunk_size) {
    if (chunk == nullptr) {
      return;
    }

    DestructAndDelete(chunk, chunk_size);
    chunk = nullptr;
  }

  void MakeChunkReady(ElemT*& chunk, const size_t to_construct) {
    chunk = static_cast<ElemT*>(::operator new(CHUNK_CAP_));
    Construct(chunk, to_construct, value_);
  }

 private:
  static const size_t MIN_CHUNK_CAP_ = 1024;
  static const size_t CHUNK_CAP_ = std::max(MIN_CHUNK_CAP_, sizeof(ElemT) * 8);
  static const size_t FULL_CHUNK_SIZE_ = CHUNK_CAP_ / sizeof(ElemT);

 private:
  DynamicStorage<ElemT*> chunks_;

  size_t size_ = 0;

  ElemT value_;

};

#endif /* chunked_storage.hpp */

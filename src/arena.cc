/*
 * @Author: victorika
 * @Date: 2025-01-20 14:36:18
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-24 12:24:28
 */
#include "arena.h"

namespace jitfusion {

uint8_t* Arena::Allocate(int64_t size) {
  if (avail_bytes_ < size) {
    auto status = AllocateChunk(std::max(size, min_chunk_size_));
    if (!status.ok()) {
      return nullptr;
    }
  }

  uint8_t* ret = avail_buf_;
  avail_buf_ += size;
  avail_bytes_ -= size;
  return ret;
}

Status Arena::Allocate(uint8_t** buf, int64_t size) {
  auto* tmp = new uint8_t[size];
  if (tmp == nullptr) {
    return Status::RuntimeError("failure to allocate");
  }
  *buf = tmp;
  return Status::OK();
}

Status Arena::Free(const uint8_t* buf) {
  if (buf == nullptr) {
    return Status::RuntimeError("failure to allocate");
  }
  delete[] buf;
  buf = nullptr;
  return Status::OK();
}

Status Arena::AllocateChunk(int64_t size) {
  uint8_t* out;

  auto status = Allocate(&out, size);
  if (!status.ok()) {
    return status;
  }
  chunks_.emplace_back(out, size);
  avail_buf_ = out;
  avail_bytes_ = size;  // left-over bytes in the previous chunk cannot be used anymore.
  total_bytes_ += size;
  return Status::OK();
}

// In the most common case, a chunk will be allocated when processing the first record.
// And, the same chunk can be used for processing the remaining records in the batch.
// By retaining the first chunk, the number of malloc calls are reduced to one per batch,
// instead of one per record.
void Arena::Reset() {
  if (chunks_.empty()) {
    // if there are no chunks, nothing to do.
    return;
  }

  // Release all but the first chunk.
  if (chunks_.size() > 1) {
    ReleaseChunks(true);
    chunks_.erase(chunks_.begin() + 1, chunks_.end());
  }

  avail_buf_ = chunks_.at(0).buf_;
  avail_bytes_ = total_bytes_ = chunks_.at(0).size_;
}

void Arena::ReleaseChunks(bool retain_first = true) {
  for (auto& chunk : chunks_) {
    if (retain_first) {
      // skip freeing first chunk.
      retain_first = false;
      continue;
    }
    Free(chunk.buf_);
  }
}

}  // namespace jitfusion
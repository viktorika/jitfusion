/*
 * @Author: victorika
 * @Date: 2025-01-20 14:36:18
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-24 12:24:28
 */
#include "arena.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace jitfusion {

uint8_t* Arena::Allocate(size_t size, size_t alignment) {
  // Zero-sized allocation is not a meaningful request and is disallowed:
  // the returned pointer could not be dereferenced safely anyway, and
  // callers asking for 0 bytes almost always indicate a bug at the call
  // site (e.g. forgetting to special-case an empty list/string). Rejecting
  // it here with an assert surfaces such bugs loudly in debug builds.
  assert(size > 0 && "Arena::Allocate called with size == 0");
  // Alignment must be a power of two and non-zero.
  assert(alignment > 0 && (alignment & (alignment - 1)) == 0);

  // Compute the padding needed to align avail_buf_ up to `alignment`.
  // Using the classic `(-addr) & mask` trick: when addr is already
  // aligned the result is 0, otherwise it equals `alignment - (addr & mask)`.
  // Works correctly even when avail_buf_ is nullptr (no chunk yet),
  // because in that case avail_bytes_ is 0 and we will fall into
  // AllocateChunk below, whose returned chunk is guaranteed to be
  // aligned to alignof(std::max_align_t) (>= 8) by malloc().
  const auto mask = static_cast<uintptr_t>(alignment) - 1;
  auto addr = reinterpret_cast<uintptr_t>(avail_buf_);
  auto padding = static_cast<size_t>((-addr) & mask);

  // Guard against pathological sizes that would wrap size_t when combined
  // with padding or alignment slack. Real call sites in this codebase only
  // ever request sizes bounded by row/batch metadata (way below SIZE_MAX),
  // so hitting either assert means a bug at the caller rather than a
  // recoverable runtime condition — fail loudly in debug, zero cost in
  // release.
  assert(size <= SIZE_MAX - padding && "Arena::Allocate size + padding overflows");

  if (avail_bytes_ < size + padding) {
    // New chunk base address from malloc() is max_align_t aligned,
    // which covers alignment values up to alignof(std::max_align_t).
    // For users requesting stricter alignment than max_align_t, we
    // over-allocate so that we can still align within the chunk.
    assert(size <= SIZE_MAX - (alignment - 1) && "Arena::Allocate size + alignment overflows");
    size_t chunk_size = std::max(size + alignment - 1, min_chunk_size_);
    auto status = AllocateChunk(chunk_size);
    if (!status.ok()) {
      return nullptr;
    }
    // Recompute padding against the fresh chunk.
    addr = reinterpret_cast<uintptr_t>(avail_buf_);
    padding = static_cast<size_t>((-addr) & mask);
  }

  uint8_t* ret = avail_buf_ + padding;
  avail_buf_ += size + padding;
  avail_bytes_ -= size + padding;
  return ret;
}

Status Arena::Allocate(uint8_t** buf, size_t size) {
  // Use std::malloc for raw byte buffer allocation. This matches the
  // "untyped bytes that will be interpreted by the caller" semantics of
  // Arena better than `new uint8_t[]`, and unlike `new` it reports
  // allocation failure via nullptr return instead of throwing, which fits
  // the non-throwing failure-propagation style used across this codebase.
  // Alignment: C standard guarantees malloc returns memory aligned to
  // alignof(std::max_align_t), same as operator new[], so chunk base
  // addresses are still suitable for all scalar types used here.
  auto* tmp = static_cast<uint8_t*>(malloc(size));
  if (tmp == nullptr) {
    return Status::RuntimeError("failure to allocate");
  }
  *buf = tmp;
  return Status::OK();
}

void Arena::Free(uint8_t* buf) {
  // Free is a private helper only invoked on buffers previously obtained
  // from the internal Allocate(uint8_t**, size_t). A null pointer here
  // means an internal invariant has been violated (e.g. a Chunk with a
  // null buf_ slipped into chunks_), which is a programming bug worth
  // catching loudly rather than silently papering over.
  assert(buf != nullptr && "Arena::Free called with nullptr (internal invariant violated)");
  free(buf);
}

Status Arena::AllocateChunk(size_t size) {
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

void Arena::ReleaseChunks(bool retain_first) {
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
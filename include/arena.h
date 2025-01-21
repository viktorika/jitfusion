/*
 * @Author: victorika
 * @Date: 2025-01-20 14:29:26
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-20 14:39:51
 */
#pragma once

#include <cstdint>
#include "status.h"

namespace jitfusion {

/// \brief Simple arena allocator.
///
/// Memory is allocated from system in units of chunk-size, and dished out in the
/// requested sizes. If the requested size > chunk-size, allocate directly from the
/// system.
///
/// The allocated memory gets released only when the arena is destroyed, or on
/// Reset.
///
/// This code is not multi-thread safe, and avoids all locking for efficiency.
///
class Arena {
 public:
  explicit Arena(int64_t min_chunk_size = 4096) : min_chunk_size_(min_chunk_size) {};
  ~Arena() { ReleaseChunks(false); }
  // Allocate buffer of requested size.
  uint8_t* Allocate(int64_t size);

  // Reset arena state.
  void Reset();

  // total bytes allocated from system.
  [[nodiscard]] int64_t total_bytes() const { return total_bytes_; }

  // total bytes available for allocations.
  [[nodiscard]] int64_t avail_bytes() const { return avail_bytes_; }

 private:
  struct Chunk {
    Chunk(uint8_t* buf, int64_t size) : buf_(buf), size_(size) {}

    uint8_t* buf_;
    int64_t size_;
  };

  static Status Allocate(uint8_t** buf, int64_t size);

  static Status Free(const uint8_t* buf);

  // Allocate new chunk.
  Status AllocateChunk(int64_t size);

  // release memory from buffers.
  void ReleaseChunks(bool retain_first);

  // The chunk-size used for allocations from system.
  int64_t min_chunk_size_;

  // Total bytes allocated from system.
  int64_t total_bytes_{};

  // Bytes available from allocated chunk.
  int64_t avail_bytes_{};

  // buffer from current chunk.
  uint8_t* avail_buf_{};

  // List of allocated chunks.
  std::vector<Chunk> chunks_;
};

}  // namespace jitfusion
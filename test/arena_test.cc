/*
 * @Author: victorika
 * @Date: 2026-04-23
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-23
 */
#include "arena.h"

#include <cstddef>
#include <cstdint>
#include <vector>

#include "gtest/gtest.h"

namespace jitfusion {

namespace {

bool IsAligned(const void* p, std::size_t alignment) {
  return (reinterpret_cast<std::uintptr_t>(p) & (alignment - 1)) == 0;
}

}  // namespace

TEST(ArenaTest, DefaultAlignmentIs8Bytes) {
  Arena arena;
  uint8_t* p1 = arena.Allocate(1);
  ASSERT_NE(p1, nullptr);
  EXPECT_TRUE(IsAligned(p1, 8));

  uint8_t* p2 = arena.Allocate(1);
  ASSERT_NE(p2, nullptr);
  EXPECT_TRUE(IsAligned(p2, 8));
  EXPECT_GE(p2 - p1, 8);
}

TEST(ArenaTest, MixedSizeAllocationsStayAligned) {
  Arena arena;
  constexpr int kRounds = 64;
  const std::vector<std::size_t> sizes = {1, 3, 5, 7, 9, 13, 17, 31, 63};
  for (int i = 0; i < kRounds; ++i) {
    for (std::size_t sz : sizes) {
      uint8_t* p = arena.Allocate(sz);
      ASSERT_NE(p, nullptr);
      EXPECT_TRUE(IsAligned(p, 8)) << "size=" << sz << " round=" << i;
      for (std::size_t j = 0; j < sz; ++j) {
        p[j] = static_cast<uint8_t>(j);
      }
    }
  }
}

TEST(ArenaTest, ExplicitAlignment) {
  Arena arena;
  (void)arena.Allocate(1);
  for (std::size_t alignment : {1U, 2U, 4U, 8U, 16U, 32U, 64U}) {
    uint8_t* p = arena.Allocate(3, alignment);
    ASSERT_NE(p, nullptr);
    EXPECT_TRUE(IsAligned(p, alignment)) << "alignment=" << alignment;
  }
}

TEST(ArenaTest, LargeAlignmentAcrossChunks) {
  Arena arena(/*min_chunk_size=*/32);
  for (int i = 0; i < 16; ++i) {
    uint8_t* p = arena.Allocate(7, 16);
    ASSERT_NE(p, nullptr);
    EXPECT_TRUE(IsAligned(p, 16));
  }
}

TEST(ArenaTest, AllocationLargerThanMinChunk) {
  Arena arena(/*min_chunk_size=*/16);
  uint8_t* p = arena.Allocate(1024, 8);
  ASSERT_NE(p, nullptr);
  EXPECT_TRUE(IsAligned(p, 8));
  // Fill it to make sure we actually own that much memory.
  for (std::size_t i = 0; i < 1024; ++i) {
    p[i] = static_cast<uint8_t>(i);
  }
}

TEST(ArenaTest, ResetKeepsAlignmentAndReusesFirstChunk) {
  Arena arena(/*min_chunk_size=*/256);
  uint8_t* p1 = arena.Allocate(100);
  ASSERT_NE(p1, nullptr);
  EXPECT_TRUE(IsAligned(p1, 8));

  arena.Reset();

  uint8_t* p2 = arena.Allocate(1);
  ASSERT_NE(p2, nullptr);
  EXPECT_TRUE(IsAligned(p2, 8));

  uint8_t* p3 = arena.Allocate(1);
  EXPECT_TRUE(IsAligned(p3, 8));
}

TEST(ArenaTest, AlignmentOne) {
  Arena arena;
  uint8_t* p1 = arena.Allocate(1, 1);
  uint8_t* p2 = arena.Allocate(1, 1);
  ASSERT_NE(p1, nullptr);
  ASSERT_NE(p2, nullptr);
  EXPECT_EQ(p2 - p1, 1);
}

TEST(ArenaTest, SimulateStringThenU64List) {
  Arena arena;
  uint8_t* str = arena.Allocate(5);
  ASSERT_NE(str, nullptr);

  auto* u64_data = reinterpret_cast<uint64_t*>(arena.Allocate(sizeof(uint64_t) * 8));
  ASSERT_NE(u64_data, nullptr);
  EXPECT_TRUE(IsAligned(u64_data, alignof(uint64_t)));

  for (int i = 0; i < 8; ++i) {
    u64_data[i] = static_cast<uint64_t>(i) * 0x0101010101010101ULL;
  }
  for (int i = 0; i < 8; ++i) {
    EXPECT_EQ(u64_data[i], static_cast<uint64_t>(i) * 0x0101010101010101ULL);
  }
}

TEST(ArenaTest, TotalAndAvailBytesAreConsistent) {
  Arena arena(/*min_chunk_size=*/128);
  EXPECT_EQ(arena.total_bytes(), 0U);
  EXPECT_EQ(arena.avail_bytes(), 0U);

  uint8_t* p = arena.Allocate(32);
  ASSERT_NE(p, nullptr);
  // First allocation triggers a chunk of >= min_chunk_size bytes.
  EXPECT_GE(arena.total_bytes(), 128U);
  // After taking out 32 bytes, avail must be total - 32 (padding for p is
  // 0 because a fresh chunk base is already aligned).
  EXPECT_EQ(arena.total_bytes() - arena.avail_bytes(), 32U);

  (void)arena.Allocate(16);
  EXPECT_EQ(arena.total_bytes() - arena.avail_bytes(), 48U);
}

TEST(ArenaTest, TotalBytesGrowsAcrossChunks) {
  Arena arena(/*min_chunk_size=*/64);

  // First chunk: 64 bytes.
  (void)arena.Allocate(40);
  size_t total_after_first = arena.total_bytes();
  EXPECT_GE(total_after_first, 64U);

  // Request that does not fit in remaining avail -> new chunk, which
  // must strictly grow total_bytes.
  (void)arena.Allocate(50);
  EXPECT_GT(arena.total_bytes(), total_after_first);
}

TEST(ArenaTest, ResetOnEmptyArenaIsSafe) {
  Arena arena;
  EXPECT_EQ(arena.total_bytes(), 0U);
  arena.Reset();  // Must not crash.
  EXPECT_EQ(arena.total_bytes(), 0U);
  arena.Reset();  // Idempotent.

  // After Reset on empty arena, normal allocation still works.
  uint8_t* p = arena.Allocate(4);
  ASSERT_NE(p, nullptr);
  EXPECT_TRUE(IsAligned(p, 8));
}

TEST(ArenaTest, ResetIsIdempotent) {
  Arena arena(/*min_chunk_size=*/64);
  // Force multiple chunks.
  for (int i = 0; i < 10; ++i) {
    (void)arena.Allocate(40);
  }
  size_t total_before_reset = arena.total_bytes();
  EXPECT_GT(total_before_reset, 64U);  // More than one chunk was allocated.

  arena.Reset();
  size_t total_after_reset = arena.total_bytes();
  // Reset releases all but the first chunk, so total drops.
  EXPECT_LT(total_after_reset, total_before_reset);
  EXPECT_EQ(total_after_reset, arena.avail_bytes());  // All of first chunk is free again.

  // A second Reset should be a no-op in terms of accounting.
  arena.Reset();
  EXPECT_EQ(arena.total_bytes(), total_after_reset);
  EXPECT_EQ(arena.avail_bytes(), total_after_reset);

  // Further allocations still work.
  uint8_t* p = arena.Allocate(8);
  ASSERT_NE(p, nullptr);
  EXPECT_TRUE(IsAligned(p, 8));
}

TEST(ArenaTest, ResetReusesFirstChunkAddress) {
  Arena arena(/*min_chunk_size=*/256);
  uint8_t* first_alloc = arena.Allocate(1);
  ASSERT_NE(first_alloc, nullptr);

  // Force more chunks.
  (void)arena.Allocate(10000);
  (void)arena.Allocate(10000);

  arena.Reset();

  // After Reset, the next allocation lands in the retained first chunk,
  // so it should return the same address as the very first allocation.
  uint8_t* p = arena.Allocate(1);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(p, first_alloc);
}

TEST(ArenaTest, AlignmentLargerThanMinChunkSize) {
  // Request alignment far larger than the initial chunk size. The arena
  // must over-allocate the chunk so the requested block still fits
  // after alignment padding.
  Arena arena(/*min_chunk_size=*/16);
  constexpr std::size_t kAlign = 64;
  uint8_t* p = arena.Allocate(8, kAlign);
  ASSERT_NE(p, nullptr);
  EXPECT_TRUE(IsAligned(p, kAlign));

  // Write into the returned block to make sure the arena actually owns it.
  for (int i = 0; i < 8; ++i) {
    p[i] = static_cast<uint8_t>(0xA0 + i);
  }
  for (int i = 0; i < 8; ++i) {
    EXPECT_EQ(p[i], static_cast<uint8_t>(0xA0 + i));
  }
}

TEST(ArenaTest, AllocationsNeverOverlap) {
  Arena arena(/*min_chunk_size=*/64);
  std::vector<std::pair<uint8_t*, std::size_t>> blocks;
  const std::vector<std::size_t> sizes = {3, 5, 11, 17, 23, 29, 31, 41};
  for (int round = 0; round < 20; ++round) {
    for (std::size_t sz : sizes) {
      uint8_t* p = arena.Allocate(sz);
      ASSERT_NE(p, nullptr);
      // Fill each block with a unique byte pattern derived from its index.
      uint8_t tag = static_cast<uint8_t>(blocks.size() & 0xFF);
      for (std::size_t i = 0; i < sz; ++i) {
        p[i] = tag;
      }
      blocks.emplace_back(p, sz);
    }
  }

  // Verify every byte of every block still holds its original tag
  // (i.e. no later allocation overwrote an earlier one).
  for (std::size_t idx = 0; idx < blocks.size(); ++idx) {
    uint8_t tag = static_cast<uint8_t>(idx & 0xFF);
    auto [p, sz] = blocks[idx];
    for (std::size_t i = 0; i < sz; ++i) {
      EXPECT_EQ(p[i], tag) << "overlap detected at block " << idx << " byte " << i;
    }
  }
}

TEST(ArenaTest, DestructorReleasesAllChunks) {
  // We can't directly observe that free() was called, but we can at least
  // exercise the destructor path on an arena that has grown past one
  // chunk, so ASan / leak sanitizers can flag leaks in that code path.
  {
    Arena arena(/*min_chunk_size=*/32);
    for (int i = 0; i < 32; ++i) {
      (void)arena.Allocate(31);
    }
    // Destructor runs here.
  }
  SUCCEED();
}

TEST(ArenaTest, AllocateZeroReturnsNullptr) {
  Arena arena;
  EXPECT_EQ(arena.Allocate(size_t{0}), nullptr);
  EXPECT_EQ(arena.Allocate(size_t{0}, size_t{1}), nullptr);
  EXPECT_EQ(arena.Allocate(size_t{0}, size_t{16}), nullptr);
  EXPECT_EQ(arena.Allocate(size_t{0}, size_t{64}), nullptr);
}

TEST(ArenaTest, AllocateZeroDoesNotConsumeChunk) {
  Arena arena(/*min_chunk_size=*/128);
  EXPECT_EQ(arena.total_bytes(), 0U);

  EXPECT_EQ(arena.Allocate(0), nullptr);
  EXPECT_EQ(arena.total_bytes(), 0U);
  EXPECT_EQ(arena.avail_bytes(), 0U);
}

TEST(ArenaTest, AllocateZeroInterleavedWithNormalAllocs) {
  Arena arena;
  uint8_t* p1 = arena.Allocate(8);
  ASSERT_NE(p1, nullptr);

  EXPECT_EQ(arena.Allocate(0), nullptr);

  uint8_t* p2 = arena.Allocate(8);
  ASSERT_NE(p2, nullptr);
  EXPECT_EQ(p2 - p1, 8);
}

TEST(ArenaTest, AllocateZeroAfterReset) {
  Arena arena;
  (void)arena.Allocate(32);
  arena.Reset();
  EXPECT_EQ(arena.Allocate(0), nullptr);
  uint8_t* p = arena.Allocate(1);
  ASSERT_NE(p, nullptr);
}

}  // namespace jitfusion

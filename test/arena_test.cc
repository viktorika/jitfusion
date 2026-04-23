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

}  // namespace jitfusion

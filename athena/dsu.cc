/*
 * @Author: victorika
 * @Date: 2026-03-30 17:11:07
 * @Last Modified by: victorika
 * @Last Modified time: 2026-03-30 17:15:47
 */
#include "dsu.h"

namespace athena {

uint64_t DSU::Find(uint64_t x) {
  auto it = parent_.find(x);
  if (it == parent_.end()) {
    parent_[x] = x;
    rank_[x] = 0;
    return x;
  }
  if (it->second != x) {
    it->second = Find(it->second);
  }
  return it->second;
}

void DSU::Union(uint64_t x, uint64_t y) {
  uint64_t rx = Find(x);
  uint64_t ry = Find(y);
  if (rx == ry) {
    return;
  }
  if (rank_[rx] < rank_[ry]) {
    parent_[rx] = ry;
  } else if (rank_[rx] > rank_[ry]) {
    parent_[ry] = rx;
  } else {
    parent_[ry] = rx;
    rank_[rx]++;
  }
}

bool DSU::Connected(uint64_t x, uint64_t y) { return Find(x) == Find(y); }

}  // namespace athena
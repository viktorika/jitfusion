/*
 * @Author: victorika
 * @Date: 2026-03-30 17:02:43
 * @Last Modified by: victorika
 * @Last Modified time: 2026-03-30 17:13:02
 */
#pragma once

#include <cstdint>
#include <unordered_map>

namespace athena {

class DSU {
 public:
  uint64_t Find(uint64_t x);
  void Union(uint64_t x, uint64_t y);
  bool Connected(uint64_t x, uint64_t y);

 private:
  std::unordered_map<uint64_t, uint64_t> parent_;
  std::unordered_map<uint64_t, uint32_t> rank_;
};

}  // namespace athena
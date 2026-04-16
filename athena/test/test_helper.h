#pragma once

#include <array>
#include <cmath>
#include "athena.h"
#include "function_registry.h"
#include "gtest/gtest.h"
#include "type.h"

namespace test {

using jitfusion::F64ListStruct;
using jitfusion::I32ListStruct;
using jitfusion::StringListStruct;
using jitfusion::StringStruct;

inline jitfusion::Arena& GetTestArena() {
  static jitfusion::Arena g_arena;
  return g_arena;
}

inline uint32_t LoadU32(void* entry_arguments, int32_t index) {
  auto* args = reinterpret_cast<uint32_t*>(entry_arguments);
  return args[index];
}

inline int32_t LoadI32(void* entry_arguments, int32_t index) {
  auto* args = reinterpret_cast<int32_t*>(entry_arguments);
  return args[index];
}

inline float LoadF32(void* entry_arguments, int32_t index) {
  auto* args = reinterpret_cast<float*>(entry_arguments);
  return args[index];
}

inline StringStruct LoadStr(void* entry_arguments, int32_t index) {
  auto* args = reinterpret_cast<std::string*>(entry_arguments);
  StringStruct result;
  result.data = args[index].data();
  result.len = args[index].size();
  return result;
}

inline StringStruct LoadNullString() {
  StringStruct result;
  result.data = nullptr;
  result.len = 0;
  return result;
}

inline I32ListStruct LoadI32List(void* entry_arguments, int32_t index) {
  auto* args = reinterpret_cast<std::vector<int32_t>*>(entry_arguments);
  I32ListStruct result;
  result.data = args[index].data();
  result.len = args[index].size();
  return result;
}

inline F64ListStruct LoadF64List(void* entry_arguments, int32_t index) {
  auto* args = reinterpret_cast<std::vector<double>*>(entry_arguments);
  F64ListStruct result;
  result.data = args[index].data();
  result.len = args[index].size();
  return result;
}

inline StringListStruct LoadStringList(void* entry_arguments, int32_t index) {
  auto* args = reinterpret_cast<std::vector<std::string>*>(entry_arguments);
  StringListStruct result;
  result.data = reinterpret_cast<StringStruct*>(GetTestArena().Allocate(sizeof(StringStruct) * args[index].size()));
  result.len = args[index].size();
  for (size_t i = 0; i < args[index].size(); ++i) {
    result.data[i].data = args[index][i].data();
    result.data[i].len = args[index][i].size();
  }
  return result;
}

inline void StoreF32(void* output, int32_t index, float value) { reinterpret_cast<float*>(output)[index] = value; }

}  // namespace test

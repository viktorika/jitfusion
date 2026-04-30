/*
 * @Author: victorika
 * @Date: 2026-04-30 11:35:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-30 11:35:00
 */
#include <cstdint>
#include <string_view>
#include <unordered_map>

#include "exec_engine.h"
#include "function_init.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {

template <typename KList>
U32ListStruct GroupIndex(KList keys, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  U32ListStruct result;
  result.len = keys.len;
  result.data = reinterpret_cast<uint32_t *>(exec_ctx->arena.Allocate(sizeof(uint32_t) * keys.len));

  std::unordered_map<typename KList::CElementType, uint32_t> table;
  table.reserve(keys.len * 2);
  uint32_t next_id = 0;
  for (uint32_t i = 0; i < keys.len; ++i) {
    auto [it, inserted] = table.try_emplace(keys.data[i], next_id);
    if (inserted) {
      ++next_id;
    }
    result.data[i] = it->second;
  }
  return result;
}

U32ListStruct GroupIndexString(StringListStruct keys, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  U32ListStruct result;
  result.len = keys.len;
  result.data = reinterpret_cast<uint32_t *>(exec_ctx->arena.Allocate(sizeof(uint32_t) * keys.len));

  std::unordered_map<std::string_view, uint32_t> table;
  table.reserve(static_cast<size_t>(keys.len) * 2);
  uint32_t next_id = 0;
  for (uint32_t i = 0; i < keys.len; ++i) {
    std::string_view sv(keys.data[i].data, keys.data[i].len);
    auto [it, inserted] = table.try_emplace(sv, next_id);
    if (inserted) {
      ++next_id;
    }
    result.data[i] = it->second;
  }
  return result;
}

}  // namespace

Status InitListGroupFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupIndex", {ValueType::kU8List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(GroupIndex<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupIndex", {ValueType::kI8List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(GroupIndex<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupIndex", {ValueType::kU16List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(GroupIndex<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupIndex", {ValueType::kI16List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(GroupIndex<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupIndex", {ValueType::kU32List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(GroupIndex<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupIndex", {ValueType::kI32List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(GroupIndex<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupIndex", {ValueType::kU64List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(GroupIndex<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupIndex", {ValueType::kI64List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(GroupIndex<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupIndex", {ValueType::kF32List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(GroupIndex<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupIndex", {ValueType::kF64List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(GroupIndex<F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupIndex", {ValueType::kStringList, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(GroupIndexString)));
  return Status::OK();
}

}  // namespace jitfusion

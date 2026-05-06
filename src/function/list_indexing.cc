/*
 * @Author: victorika
 * @Date: 2026-05-06 11:15:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-06 11:15:00
 */
#include <cstdint>
#include <limits>
#include <string_view>
#include <unordered_map>

#include "exec_engine.h"
#include "function_init.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {

constexpr uint32_t kLookupMiss = std::numeric_limits<uint32_t>::max();

template <typename KList>
U32ListStruct ListLookupIndex(KList a, KList b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  U32ListStruct result;
  result.len = a.len;
  result.data = reinterpret_cast<uint32_t *>(exec_ctx->arena.Allocate(sizeof(uint32_t) * a.len));

  std::unordered_map<typename KList::CElementType, uint32_t> table;
  table.reserve(static_cast<size_t>(b.len) * 2);
  for (uint32_t j = 0; j < b.len; ++j) {
    table.try_emplace(b.data[j], j);
  }

  for (uint32_t i = 0; i < a.len; ++i) {
    auto it = table.find(a.data[i]);
    result.data[i] = (it == table.end()) ? kLookupMiss : it->second;
  }
  return result;
}

U32ListStruct ListLookupIndexString(StringListStruct a, StringListStruct b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  U32ListStruct result;
  result.len = a.len;
  result.data = reinterpret_cast<uint32_t *>(exec_ctx->arena.Allocate(sizeof(uint32_t) * a.len));

  std::unordered_map<std::string_view, uint32_t> table;
  table.reserve(static_cast<size_t>(b.len) * 2);
  for (uint32_t j = 0; j < b.len; ++j) {
    std::string_view sv(b.data[j].data, b.data[j].len);
    table.try_emplace(sv, j);
  }

  for (uint32_t i = 0; i < a.len; ++i) {
    std::string_view sv(a.data[i].data, a.data[i].len);
    auto it = table.find(sv);
    result.data[i] = (it == table.end()) ? kLookupMiss : it->second;
  }
  return result;
}

U32ListStruct ListCompactPositions(U32ListStruct raw, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  uint32_t hits = 0;
  for (uint32_t i = 0; i < raw.len; ++i) {
    hits += (raw.data[i] != kLookupMiss) ? 1U : 0U;
  }

  U32ListStruct result;
  result.len = hits;
  result.data = reinterpret_cast<uint32_t *>(exec_ctx->arena.Allocate(sizeof(uint32_t) * hits));

  uint32_t out = 0;
  for (uint32_t i = 0; i < raw.len; ++i) {
    if (raw.data[i] != kLookupMiss) {
      result.data[out++] = i;
    }
  }
  return result;
}

U32ListStruct ListCompactIndex(U32ListStruct raw, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);

  uint32_t hits = 0;
  for (uint32_t i = 0; i < raw.len; ++i) {
    hits += (raw.data[i] != kLookupMiss) ? 1U : 0U;
  }

  U32ListStruct result;
  result.len = hits;
  result.data = reinterpret_cast<uint32_t *>(exec_ctx->arena.Allocate(sizeof(uint32_t) * hits));

  uint32_t out = 0;
  for (uint32_t i = 0; i < raw.len; ++i) {
    if (raw.data[i] != kLookupMiss) {
      result.data[out++] = raw.data[i];
    }
  }
  return result;
}

template <typename VList>
VList ListGather(VList values, U32ListStruct idx, typename VList::CElementType default_value, void *exec_context) {
  using V = typename VList::CElementType;
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  VList result;
  result.len = idx.len;
  result.data = reinterpret_cast<V *>(exec_ctx->arena.Allocate(sizeof(V) * idx.len));

  for (uint32_t k = 0; k < idx.len; ++k) {
    uint32_t j = idx.data[k];
    result.data[k] = (j < values.len) ? values.data[j] : default_value;
  }
  return result;
}

Status InitListLookupIndexFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLookupIndex", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListLookupIndex<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLookupIndex", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListLookupIndex<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLookupIndex", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListLookupIndex<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLookupIndex", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListLookupIndex<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLookupIndex", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListLookupIndex<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLookupIndex", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListLookupIndex<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLookupIndex", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListLookupIndex<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLookupIndex", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListLookupIndex<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLookupIndex", {ValueType::kF32List, ValueType::kF32List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListLookupIndex<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLookupIndex", {ValueType::kF64List, ValueType::kF64List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListLookupIndex<F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLookupIndex", {ValueType::kStringList, ValueType::kStringList, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListLookupIndexString)));
  return Status::OK();
}

Status InitListCompactFuncs(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListCompactPositions", {ValueType::kU32List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(ListCompactPositions)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListCompactIndex", {ValueType::kU32List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(ListCompactIndex)));
  return Status::OK();
}

Status InitListGatherFunc(FunctionRegistry *reg) {
  // Signature: ListGather(values, idx, default, ctx)
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListGather", {ValueType::kU8List, ValueType::kU32List, ValueType::kU8, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(ListGather<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListGather", {ValueType::kI8List, ValueType::kU32List, ValueType::kI8, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(ListGather<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListGather", {ValueType::kU16List, ValueType::kU32List, ValueType::kU16, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(ListGather<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListGather", {ValueType::kI16List, ValueType::kU32List, ValueType::kI16, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(ListGather<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListGather", {ValueType::kU32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListGather<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListGather", {ValueType::kI32List, ValueType::kU32List, ValueType::kI32, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(ListGather<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListGather", {ValueType::kU64List, ValueType::kU32List, ValueType::kU64, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(ListGather<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListGather", {ValueType::kI64List, ValueType::kU32List, ValueType::kI64, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(ListGather<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListGather", {ValueType::kF32List, ValueType::kU32List, ValueType::kF32, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(ListGather<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListGather", {ValueType::kF64List, ValueType::kU32List, ValueType::kF64, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(ListGather<F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListGather",
                        {ValueType::kStringList, ValueType::kU32List, ValueType::kString, ValueType::kPtr},
                        ValueType::kStringList),
      reinterpret_cast<void *>(ListGather<StringListStruct>)));
  return Status::OK();
}

}  // namespace

Status InitListIndexingFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(InitListLookupIndexFunc(reg));
  JF_RETURN_NOT_OK(InitListCompactFuncs(reg));
  JF_RETURN_NOT_OK(InitListGatherFunc(reg));
  return Status::OK();
}

}  // namespace jitfusion

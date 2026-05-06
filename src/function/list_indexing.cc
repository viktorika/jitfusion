/*
 * @Author: victorika
 * @Date: 2026-05-06 11:15:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-06 11:15:00
 */
#include <algorithm>
#include <cstdint>
#include <limits>
#include <string_view>
#include <unordered_map>

#include "codegen/codegen.h"
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

template <typename KList>
uint32_t Find(KList a, typename KList::CElementType value) {
  for (uint32_t i = 0; i < a.len; ++i) {
    if (a.data[i] == value) {
      return i;
    }
  }
  return kLookupMiss;
}

uint32_t FindString(StringListStruct a, StringStruct value) {
  std::string_view needle(value.data, value.len);
  for (uint32_t i = 0; i < a.len; ++i) {
    std::string_view sv(a.data[i].data, a.data[i].len);
    if (sv == needle) {
      return i;
    }
  }
  return kLookupMiss;
}

template <typename KList>
uint32_t FindSorted(KList a, typename KList::CElementType value) {
  const auto *begin = a.data;
  const auto *end = a.data + a.len;
  const auto *it = std::lower_bound(begin, end, value);
  if (it == end || *it != value) {
    return kLookupMiss;
  }
  return static_cast<uint32_t>(it - begin);
}

uint32_t FindSortedString(StringListStruct a, StringStruct value) {
  std::string_view needle(value.data, value.len);
  const auto *begin = a.data;
  const auto *end = a.data + a.len;
  const auto *it = std::lower_bound(begin, end, needle, [](const StringStruct &lhs, std::string_view rhs) {
    return std::string_view(lhs.data, lhs.len) < rhs;
  });
  if (it == end || std::string_view(it->data, it->len) != needle) {
    return kLookupMiss;
  }
  return static_cast<uint32_t>(it - begin);
}

llvm::Value *CodegenFindMiss(const FunctionSignature & /*sign*/, const std::vector<llvm::Type *> & /*arg_types*/,
                             const std::vector<llvm::Value *> & /*args*/, IRCodeGenContext &ctx) {
  return ctx.builder.getInt32(kLookupMiss);
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

constexpr uint32_t kBucketizeLinearThreshold = 16;

template <typename ListType>
U32ListStruct Bucketize(ListType values, ListType boundaries, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  U32ListStruct result;
  result.len = values.len;
  result.data = reinterpret_cast<uint32_t *>(exec_ctx->arena.Allocate(sizeof(uint32_t) * values.len));

  const auto *bbegin = boundaries.data;
  const uint32_t bn = boundaries.len;

  if (bn <= kBucketizeLinearThreshold) {
    for (uint32_t i = 0; i < values.len; ++i) {
      const auto v = values.data[i];
      uint32_t bucket = 0;
      // `!(v < b)` is intentional, do NOT simplify to `v >= b` or `b <= v`:
      // upper_bound uses the same `value < *it` predicate, so for NaN — where
      // every comparison is false — both paths walk past the end and land in
      // the top bucket. Switching to `>=` would map NaN to bucket 0 and break
      // bit-for-bit parity with the binary-search path.
      while (bucket < bn && !(v < bbegin[bucket])) {
        ++bucket;
      }
      result.data[i] = bucket;
    }
  } else {
    const auto *bend = bbegin + bn;
    for (uint32_t i = 0; i < values.len; ++i) {
      const auto *it = std::upper_bound(bbegin, bend, values.data[i]);
      result.data[i] = static_cast<uint32_t>(it - bbegin);
    }
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

Status InitFindFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("Find", {ValueType::kU8List, ValueType::kU8}, ValueType::kU32),
                                 reinterpret_cast<void *>(Find<U8ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("Find", {ValueType::kI8List, ValueType::kI8}, ValueType::kU32),
                                 reinterpret_cast<void *>(Find<I8ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("Find", {ValueType::kU16List, ValueType::kU16}, ValueType::kU32),
                                 reinterpret_cast<void *>(Find<U16ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("Find", {ValueType::kI16List, ValueType::kI16}, ValueType::kU32),
                                 reinterpret_cast<void *>(Find<I16ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("Find", {ValueType::kU32List, ValueType::kU32}, ValueType::kU32),
                                 reinterpret_cast<void *>(Find<U32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("Find", {ValueType::kI32List, ValueType::kI32}, ValueType::kU32),
                                 reinterpret_cast<void *>(Find<I32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("Find", {ValueType::kU64List, ValueType::kU64}, ValueType::kU32),
                                 reinterpret_cast<void *>(Find<U64ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("Find", {ValueType::kI64List, ValueType::kI64}, ValueType::kU32),
                                 reinterpret_cast<void *>(Find<I64ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("Find", {ValueType::kF32List, ValueType::kF32}, ValueType::kU32),
                                 reinterpret_cast<void *>(Find<F32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("Find", {ValueType::kF64List, ValueType::kF64}, ValueType::kU32),
                                 reinterpret_cast<void *>(Find<F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("Find", {ValueType::kStringList, ValueType::kString}, ValueType::kU32),
      reinterpret_cast<void *>(FindString)));
  return Status::OK();
}

Status InitFindSortedFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("FindSorted", {ValueType::kU8List, ValueType::kU8}, ValueType::kU32),
                                 reinterpret_cast<void *>(FindSorted<U8ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("FindSorted", {ValueType::kI8List, ValueType::kI8}, ValueType::kU32),
                                 reinterpret_cast<void *>(FindSorted<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FindSorted", {ValueType::kU16List, ValueType::kU16}, ValueType::kU32),
      reinterpret_cast<void *>(FindSorted<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FindSorted", {ValueType::kI16List, ValueType::kI16}, ValueType::kU32),
      reinterpret_cast<void *>(FindSorted<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FindSorted", {ValueType::kU32List, ValueType::kU32}, ValueType::kU32),
      reinterpret_cast<void *>(FindSorted<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FindSorted", {ValueType::kI32List, ValueType::kI32}, ValueType::kU32),
      reinterpret_cast<void *>(FindSorted<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FindSorted", {ValueType::kU64List, ValueType::kU64}, ValueType::kU32),
      reinterpret_cast<void *>(FindSorted<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FindSorted", {ValueType::kI64List, ValueType::kI64}, ValueType::kU32),
      reinterpret_cast<void *>(FindSorted<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FindSorted", {ValueType::kF32List, ValueType::kF32}, ValueType::kU32),
      reinterpret_cast<void *>(FindSorted<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FindSorted", {ValueType::kF64List, ValueType::kF64}, ValueType::kU32),
      reinterpret_cast<void *>(FindSorted<F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FindSorted", {ValueType::kStringList, ValueType::kString}, ValueType::kU32),
      reinterpret_cast<void *>(FindSortedString)));
  return Status::OK();
}

Status InitFindMissFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(FunctionSignature("FindMiss", {}, ValueType::kU32), CodegenFindMiss));
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

Status InitBucketizeFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("Bucketize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(Bucketize<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("Bucketize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(Bucketize<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("Bucketize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(Bucketize<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("Bucketize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(Bucketize<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("Bucketize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(Bucketize<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("Bucketize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(Bucketize<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("Bucketize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(Bucketize<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("Bucketize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(Bucketize<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("Bucketize", {ValueType::kF32List, ValueType::kF32List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(Bucketize<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("Bucketize", {ValueType::kF64List, ValueType::kF64List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(Bucketize<F64ListStruct>)));
  return Status::OK();
}

}  // namespace

Status InitListIndexingFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(InitListLookupIndexFunc(reg));
  JF_RETURN_NOT_OK(InitFindFunc(reg));
  JF_RETURN_NOT_OK(InitFindSortedFunc(reg));
  JF_RETURN_NOT_OK(InitFindMissFunc(reg));
  JF_RETURN_NOT_OK(InitListCompactFuncs(reg));
  JF_RETURN_NOT_OK(InitListGatherFunc(reg));
  JF_RETURN_NOT_OK(InitBucketizeFunc(reg));
  return Status::OK();
}

}  // namespace jitfusion

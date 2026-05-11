/*
 * @Author: victorika
 * @Date: 2026-05-06 11:15:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-11 14:55:04
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
using IntLookupTable = std::unordered_map<typename KList::CElementType, uint32_t>;
using StringLookupTable = std::unordered_map<std::string_view, uint32_t>;

template <typename KList>
void *BuildLookupTableInt(KList b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  IntLookupTable<KList> seed;
  seed.reserve(static_cast<size_t>(b.len) * 2);
  for (uint32_t j = 0; j < b.len; ++j) {
    seed.try_emplace(b.data[j], j);
  }
  return exec_ctx->arena.Create<IntLookupTable<KList>>(std::move(seed));
}

void *BuildLookupTableString(StringListStruct b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  StringLookupTable seed;
  seed.reserve(static_cast<size_t>(b.len) * 2);
  for (uint32_t j = 0; j < b.len; ++j) {
    std::string_view sv(b.data[j].data, b.data[j].len);
    seed.try_emplace(sv, j);
  }
  return exec_ctx->arena.Create<StringLookupTable>(std::move(seed));
}

template <typename KList>
U32ListStruct LookupTableInt(KList a, void *table_ptr, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  const auto *table = reinterpret_cast<const IntLookupTable<KList> *>(table_ptr);

  U32ListStruct result;
  result.len = a.len;
  result.data = reinterpret_cast<uint32_t *>(exec_ctx->arena.Allocate(sizeof(uint32_t) * a.len));
  for (uint32_t i = 0; i < a.len; ++i) {
    auto it = table->find(a.data[i]);
    result.data[i] = (it == table->end()) ? kLookupMiss : it->second;
  }
  return result;
}

U32ListStruct LookupTableString(StringListStruct a, void *table_ptr, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  const auto *table = reinterpret_cast<const StringLookupTable *>(table_ptr);

  U32ListStruct result;
  result.len = a.len;
  result.data = reinterpret_cast<uint32_t *>(exec_ctx->arena.Allocate(sizeof(uint32_t) * a.len));
  for (uint32_t i = 0; i < a.len; ++i) {
    std::string_view sv(a.data[i].data, a.data[i].len);
    auto it = table->find(sv);
    result.data[i] = (it == table->end()) ? kLookupMiss : it->second;
  }
  return result;
}

// Internal symbol names exposed on FunctionRegistry. They are not meant
// to be invoked directly from user expressions (the user-facing API is
// `ListLookupIndex`); the lowering function below references them via
// these exact names, and `MappingToJIT` resolves the names to the C
// pointers registered alongside.
constexpr const char *kBuildIntName = "__BuildLookupTableInt";
constexpr const char *kBuildStringName = "__BuildLookupTableString";
constexpr const char *kLookupIntName = "__LookupTableInt";
constexpr const char *kLookupStringName = "__LookupTableString";

bool IsStringListType(ValueType v) { return v == ValueType::kStringList; }

// Lowering for the user-facing ListLookupIndex(a, b, ctx). Emits two
// readonly C calls - one to build the hash table over `b`, one to probe
// the table with `a`. LLVM CSE merges the BuildLookupTable_* call across
// multiple ListLookupIndex sites that share the same `b`.
llvm::Value *CodegenListLookupIndex(const FunctionSignature &sign, const std::vector<llvm::Type *> &arg_types,
                                    const std::vector<llvm::Value *> &args, IRCodeGenContext &ctx) {
  // sign.GetParamTypes() = {KList, KList, kPtr}; args[0]=a, args[1]=b, args[2]=exec_ctx
  const auto &param_types = sign.GetParamTypes();
  const ValueType list_vt = param_types[0];
  const bool is_string = IsStringListType(list_vt);
  const char *build_name = is_string ? kBuildStringName : kBuildIntName;
  const char *lookup_name = is_string ? kLookupStringName : kLookupIntName;

  // Each helper has its own *registered* signature (so MappingToJIT and
  // SetCFuncAttr can find them). Mirror those signatures here when
  // asking the module for the function declaration.
  FunctionSignature build_sign(build_name, {list_vt, ValueType::kPtr}, ValueType::kPtr);
  FunctionSignature lookup_sign(lookup_name, {list_vt, ValueType::kPtr, ValueType::kPtr}, ValueType::kU32List);

  llvm::Type *ptr_ty = llvm::PointerType::getUnqual(ctx.context);
  llvm::Type *list_ty = arg_types[0];  // matches args[0] / args[1] (KList struct value)
  llvm::Type *u32list_ty = ctx.complex_type.u32list_type;

  // Build call: ptr (KList, ptr)
  auto *build_fty = llvm::FunctionType::get(ptr_ty, {list_ty, ptr_ty}, false);
  llvm::FunctionCallee build_callee = ctx.module.getOrInsertFunction(build_sign.ToString(), build_fty);
  llvm::Value *table_ptr = ctx.builder.CreateCall(build_callee, {args[1], args[2]}, "call_BuildLookupTable");

  // Lookup call: U32List (KList, ptr, ptr)
  auto *lookup_fty = llvm::FunctionType::get(u32list_ty, {list_ty, ptr_ty, ptr_ty}, false);
  llvm::FunctionCallee lookup_callee = ctx.module.getOrInsertFunction(lookup_sign.ToString(), lookup_fty);
  llvm::Value *result = ctx.builder.CreateCall(lookup_callee, {args[0], table_ptr, args[2]}, "call_LookupTable");
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
      FunctionSignature(kBuildIntName, {ValueType::kU8List, ValueType::kPtr}, ValueType::kPtr),
      reinterpret_cast<void *>(BuildLookupTableInt<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kLookupIntName, {ValueType::kU8List, ValueType::kPtr, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(LookupTableInt<U8ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kBuildIntName, {ValueType::kI8List, ValueType::kPtr}, ValueType::kPtr),
      reinterpret_cast<void *>(BuildLookupTableInt<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kLookupIntName, {ValueType::kI8List, ValueType::kPtr, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(LookupTableInt<I8ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kBuildIntName, {ValueType::kU16List, ValueType::kPtr}, ValueType::kPtr),
      reinterpret_cast<void *>(BuildLookupTableInt<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kLookupIntName, {ValueType::kU16List, ValueType::kPtr, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(LookupTableInt<U16ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kBuildIntName, {ValueType::kI16List, ValueType::kPtr}, ValueType::kPtr),
      reinterpret_cast<void *>(BuildLookupTableInt<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kLookupIntName, {ValueType::kI16List, ValueType::kPtr, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(LookupTableInt<I16ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kBuildIntName, {ValueType::kU32List, ValueType::kPtr}, ValueType::kPtr),
      reinterpret_cast<void *>(BuildLookupTableInt<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kLookupIntName, {ValueType::kU32List, ValueType::kPtr, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(LookupTableInt<U32ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kBuildIntName, {ValueType::kI32List, ValueType::kPtr}, ValueType::kPtr),
      reinterpret_cast<void *>(BuildLookupTableInt<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kLookupIntName, {ValueType::kI32List, ValueType::kPtr, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(LookupTableInt<I32ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kBuildIntName, {ValueType::kU64List, ValueType::kPtr}, ValueType::kPtr),
      reinterpret_cast<void *>(BuildLookupTableInt<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kLookupIntName, {ValueType::kU64List, ValueType::kPtr, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(LookupTableInt<U64ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kBuildIntName, {ValueType::kI64List, ValueType::kPtr}, ValueType::kPtr),
      reinterpret_cast<void *>(BuildLookupTableInt<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kLookupIntName, {ValueType::kI64List, ValueType::kPtr, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(LookupTableInt<I64ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kBuildIntName, {ValueType::kF32List, ValueType::kPtr}, ValueType::kPtr),
      reinterpret_cast<void *>(BuildLookupTableInt<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kLookupIntName, {ValueType::kF32List, ValueType::kPtr, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(LookupTableInt<F32ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kBuildIntName, {ValueType::kF64List, ValueType::kPtr}, ValueType::kPtr),
      reinterpret_cast<void *>(BuildLookupTableInt<F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kLookupIntName, {ValueType::kF64List, ValueType::kPtr, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(LookupTableInt<F64ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kBuildStringName, {ValueType::kStringList, ValueType::kPtr}, ValueType::kPtr),
      reinterpret_cast<void *>(BuildLookupTableString)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature(kLookupStringName, {ValueType::kStringList, ValueType::kPtr, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(LookupTableString)));

  for (ValueType vt : {ValueType::kU8List, ValueType::kI8List, ValueType::kU16List, ValueType::kI16List,
                       ValueType::kU32List, ValueType::kI32List, ValueType::kU64List, ValueType::kI64List,
                       ValueType::kF32List, ValueType::kF64List, ValueType::kStringList}) {
    JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
        FunctionSignature("ListLookupIndex", {vt, vt, ValueType::kPtr}, ValueType::kU32List), CodegenListLookupIndex));
  }
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

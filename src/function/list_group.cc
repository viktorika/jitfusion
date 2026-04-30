/*
 * @Author: victorika
 * @Date: 2026-04-30 11:35:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-30 11:35:00
 */
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "codegen/codegen.h"
#include "exec_engine.h"
#include "function_init.h"
#include "function_registry.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
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

uint32_t GroupCount(U32ListStruct group_index) {
  if (group_index.len == 0) {
    return 0;
  }
  uint32_t max_id = 0;
  for (uint32_t i = 0; i < group_index.len; ++i) {
    max_id = std::max(group_index.data[i], max_id);
  }
  return max_id + 1;
}

template <typename KList>
KList GroupKeys(KList keys, U32ListStruct group_index, uint32_t distinct, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  if (keys.len != group_index.len) {
    exec_ctx->AddError("GroupKeys: keys and group_index len mismatch, keys len: " + std::to_string(keys.len) +
                       ", group_index len: " + std::to_string(group_index.len));
    return {nullptr, 0};
  }
  KList result;
  result.data = reinterpret_cast<typename KList::CElementType *>(
      exec_ctx->arena.Allocate(sizeof(typename KList::CElementType) * distinct));
  uint32_t filled = 0;
  for (uint32_t i = 0; i < keys.len; ++i) {
    if (group_index.data[i] == filled) {
      result.data[filled++] = keys.data[i];
    }
  }
  result.len = filled;
  return result;
}

template <typename AccList, typename VList>
AccList GroupSum(VList values, U32ListStruct group_index, uint32_t distinct, void *exec_context) {
  using Acc = typename AccList::CElementType;
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  if (values.len != group_index.len) {
    exec_ctx->AddError("GroupSum: values and group_index len mismatch, values len: " + std::to_string(values.len) +
                       ", group_index len: " + std::to_string(group_index.len));
    return {nullptr, 0};
  }
  AccList result;
  result.len = distinct;
  result.data = reinterpret_cast<Acc *>(exec_ctx->arena.Allocate(sizeof(Acc) * distinct));
  std::memset(result.data, 0, sizeof(Acc) * distinct);
  for (uint32_t i = 0; i < values.len; ++i) {
    result.data[group_index.data[i]] += values.data[i];
  }
  return result;
}

template <typename VList>
VList GroupMax(VList values, U32ListStruct group_index, uint32_t distinct, void *exec_context) {
  using V = typename VList::CElementType;
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  if (values.len != group_index.len) {
    exec_ctx->AddError("GroupMax: values and group_index len mismatch, values len: " + std::to_string(values.len) +
                       ", group_index len: " + std::to_string(group_index.len));
    return {nullptr, 0};
  }
  VList result;
  result.len = distinct;
  result.data = reinterpret_cast<V *>(exec_ctx->arena.Allocate(sizeof(V) * distinct));
  V seed = std::numeric_limits<V>::lowest();
  std::fill_n(result.data, distinct, seed);
  for (uint32_t i = 0; i < values.len; ++i) {
    result.data[group_index.data[i]] = std::max(result.data[group_index.data[i]], values.data[i]);
  }
  return result;
}

template <typename VList>
VList GroupMin(VList values, U32ListStruct group_index, uint32_t distinct, void *exec_context) {
  using V = typename VList::CElementType;
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  if (values.len != group_index.len) {
    exec_ctx->AddError("GroupMin: values and group_index len mismatch, values len: " + std::to_string(values.len) +
                       ", group_index len: " + std::to_string(group_index.len));
    return {nullptr, 0};
  }
  VList result;
  result.len = distinct;
  result.data = reinterpret_cast<V *>(exec_ctx->arena.Allocate(sizeof(V) * distinct));
  V seed = std::numeric_limits<V>::max();
  std::fill_n(result.data, distinct, seed);
  for (uint32_t i = 0; i < values.len; ++i) {
    result.data[group_index.data[i]] = std::min(result.data[group_index.data[i]], values.data[i]);
  }
  return result;
}

template <typename VList>
F64ListStruct GroupAvg(VList values, U32ListStruct group_index, uint32_t distinct, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  if (values.len != group_index.len) {
    exec_ctx->AddError("GroupAvg: values and group_index len mismatch, values len: " + std::to_string(values.len) +
                       ", group_index len: " + std::to_string(group_index.len));
    return {nullptr, 0};
  }
  F64ListStruct result;
  result.len = distinct;
  result.data = reinterpret_cast<double *>(exec_ctx->arena.Allocate(sizeof(double) * distinct));
  std::memset(result.data, 0, sizeof(double) * distinct);
  std::vector<uint32_t> local_counts(distinct, 0);
  for (uint32_t i = 0; i < values.len; ++i) {
    auto gid = group_index.data[i];
    result.data[gid] += values.data[i];
    local_counts[gid] += 1;
  }
  for (uint32_t g = 0; g < distinct; ++g) {
    result.data[g] /= local_counts[g];
  }
  return result;
}

Status InitGroupIndexFunc(FunctionRegistry *reg) {
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

Status InitGroupCountFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("GroupCount", {ValueType::kU32List}, ValueType::kU32),
                                              reinterpret_cast<void *>(GroupCount)));
  return Status::OK();
}

Status InitGroupKeysFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupKeys", {ValueType::kU8List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GroupKeys<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupKeys", {ValueType::kI8List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(GroupKeys<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupKeys", {ValueType::kU16List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(GroupKeys<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupKeys", {ValueType::kI16List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(GroupKeys<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupKeys", {ValueType::kU32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(GroupKeys<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupKeys", {ValueType::kI32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(GroupKeys<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupKeys", {ValueType::kU64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(GroupKeys<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupKeys", {ValueType::kI64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(GroupKeys<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupKeys", {ValueType::kF32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(GroupKeys<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupKeys", {ValueType::kF64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupKeys<F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupKeys", {ValueType::kStringList, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kStringList),
      reinterpret_cast<void *>(GroupKeys<StringListStruct>)));
  return Status::OK();
}

llvm::Value *EmitGroupKeysSugar(const FunctionSignature &sign, const std::vector<llvm::Type *> &arg_llvm_type_list,
                                const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *keys_value = arg_llvm_value_list.at(0);
  auto *group_index_value = arg_llvm_value_list.at(1);
  auto *exec_context_value = arg_llvm_value_list.at(2);
  auto *keys_llvm_type = arg_llvm_type_list.at(0);

  auto *i32_ty = llvm::Type::getInt32Ty(ctx.context);
  auto *ptr_ty = llvm::PointerType::getUnqual(ctx.context);
  auto *u32list_llvm_type = ctx.complex_type.u32list_type;

  FunctionSignature group_count_sign("GroupCount", {ValueType::kU32List}, ValueType::kU32);
  auto *group_count_func_type = llvm::FunctionType::get(i32_ty, {u32list_llvm_type}, false);
  llvm::FunctionCallee group_count_callee =
      ctx.module.getOrInsertFunction(group_count_sign.ToString(), group_count_func_type);
  llvm::Value *distinct_value = ctx.builder.CreateCall(group_count_callee, {group_index_value}, "group_count");

  FunctionSignature group_keys_sign("GroupKeys",
                                    {sign.GetParamTypes().at(0), ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                                    sign.GetRetType());
  auto *group_keys_func_type =
      llvm::FunctionType::get(keys_llvm_type, {keys_llvm_type, u32list_llvm_type, i32_ty, ptr_ty}, false);
  llvm::FunctionCallee group_keys_callee =
      ctx.module.getOrInsertFunction(group_keys_sign.ToString(), group_keys_func_type);
  return ctx.builder.CreateCall(group_keys_callee, {keys_value, group_index_value, distinct_value, exec_context_value},
                                "group_keys");
}

Status InitGroupKeysSugarFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupKeys", {ValueType::kU8List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU8List),
      EmitGroupKeysSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupKeys", {ValueType::kI8List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI8List),
      EmitGroupKeysSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupKeys", {ValueType::kU16List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU16List),
      EmitGroupKeysSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupKeys", {ValueType::kI16List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI16List),
      EmitGroupKeysSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupKeys", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU32List),
      EmitGroupKeysSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupKeys", {ValueType::kI32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI32List),
      EmitGroupKeysSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupKeys", {ValueType::kU64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU64List),
      EmitGroupKeysSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupKeys", {ValueType::kI64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI64List),
      EmitGroupKeysSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupKeys", {ValueType::kF32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF32List),
      EmitGroupKeysSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupKeys", {ValueType::kF64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupKeysSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupKeys", {ValueType::kStringList, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kStringList),
      EmitGroupKeysSugar));
  return Status::OK();
}

Status InitGroupSumFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupSum", {ValueType::kI8List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(GroupSum<I64ListStruct, I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupSum", {ValueType::kI16List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(GroupSum<I64ListStruct, I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupSum", {ValueType::kI32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(GroupSum<I64ListStruct, I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupSum", {ValueType::kI64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(GroupSum<I64ListStruct, I64ListStruct>)));
  // Unsigned integers promote to u64.
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupSum", {ValueType::kU8List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(GroupSum<U64ListStruct, U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupSum", {ValueType::kU16List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(GroupSum<U64ListStruct, U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupSum", {ValueType::kU32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(GroupSum<U64ListStruct, U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupSum", {ValueType::kU64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(GroupSum<U64ListStruct, U64ListStruct>)));
  // Floating-point promotes to f64.
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupSum", {ValueType::kF32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupSum<F64ListStruct, F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupSum", {ValueType::kF64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupSum<F64ListStruct, F64ListStruct>)));
  return Status::OK();
}

llvm::Value *EmitGroupSumSugar(const FunctionSignature &sign, const std::vector<llvm::Type *> &arg_llvm_type_list,
                               const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *values_value = arg_llvm_value_list.at(0);
  auto *group_index_value = arg_llvm_value_list.at(1);
  auto *exec_context_value = arg_llvm_value_list.at(2);
  auto *values_llvm_type = arg_llvm_type_list.at(0);

  auto *i32_ty = llvm::Type::getInt32Ty(ctx.context);
  auto *ptr_ty = llvm::PointerType::getUnqual(ctx.context);
  auto *u32list_llvm_type = ctx.complex_type.u32list_type;

  llvm::Type *acc_llvm_type = nullptr;
  (void)CodeGen::ValueTypeToLLVMType(ctx, sign.GetRetType(), &acc_llvm_type);

  FunctionSignature group_count_sign("GroupCount", {ValueType::kU32List}, ValueType::kU32);
  auto *group_count_func_type = llvm::FunctionType::get(i32_ty, {u32list_llvm_type}, false);
  llvm::FunctionCallee group_count_callee =
      ctx.module.getOrInsertFunction(group_count_sign.ToString(), group_count_func_type);
  llvm::Value *distinct_value = ctx.builder.CreateCall(group_count_callee, {group_index_value}, "group_count");

  FunctionSignature group_sum_sign("GroupSum",
                                   {sign.GetParamTypes().at(0), ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                                   sign.GetRetType());
  auto *group_sum_func_type =
      llvm::FunctionType::get(acc_llvm_type, {values_llvm_type, u32list_llvm_type, i32_ty, ptr_ty}, false);
  llvm::FunctionCallee group_sum_callee =
      ctx.module.getOrInsertFunction(group_sum_sign.ToString(), group_sum_func_type);
  return ctx.builder.CreateCall(group_sum_callee, {values_value, group_index_value, distinct_value, exec_context_value},
                                "group_sum");
}

Status InitGroupSumSugarFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupSum", {ValueType::kI8List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI64List),
      EmitGroupSumSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupSum", {ValueType::kI16List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI64List),
      EmitGroupSumSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupSum", {ValueType::kI32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI64List),
      EmitGroupSumSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupSum", {ValueType::kI64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI64List),
      EmitGroupSumSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupSum", {ValueType::kU8List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU64List),
      EmitGroupSumSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupSum", {ValueType::kU16List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU64List),
      EmitGroupSumSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupSum", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU64List),
      EmitGroupSumSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupSum", {ValueType::kU64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU64List),
      EmitGroupSumSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupSum", {ValueType::kF32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupSumSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupSum", {ValueType::kF64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupSumSugar));
  return Status::OK();
}

Status InitGroupMaxFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMax", {ValueType::kU8List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GroupMax<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMax", {ValueType::kI8List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(GroupMax<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMax", {ValueType::kU16List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(GroupMax<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMax", {ValueType::kI16List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(GroupMax<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMax", {ValueType::kU32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(GroupMax<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMax", {ValueType::kI32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(GroupMax<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMax", {ValueType::kU64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(GroupMax<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMax", {ValueType::kI64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(GroupMax<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMax", {ValueType::kF32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(GroupMax<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMax", {ValueType::kF64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupMax<F64ListStruct>)));
  return Status::OK();
}

Status InitGroupMinFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMin", {ValueType::kU8List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GroupMin<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMin", {ValueType::kI8List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(GroupMin<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMin", {ValueType::kU16List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(GroupMin<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMin", {ValueType::kI16List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(GroupMin<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMin", {ValueType::kU32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(GroupMin<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMin", {ValueType::kI32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(GroupMin<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMin", {ValueType::kU64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(GroupMin<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMin", {ValueType::kI64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(GroupMin<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMin", {ValueType::kF32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(GroupMin<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupMin", {ValueType::kF64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupMin<F64ListStruct>)));
  return Status::OK();
}

llvm::Value *EmitGroupExtremaSugar(const std::string &kernel_name, const FunctionSignature &sign,
                                   const std::vector<llvm::Type *> &arg_llvm_type_list,
                                   const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx,
                                   const char *call_name) {
  auto *values_value = arg_llvm_value_list.at(0);
  auto *group_index_value = arg_llvm_value_list.at(1);
  auto *exec_context_value = arg_llvm_value_list.at(2);
  auto *values_llvm_type = arg_llvm_type_list.at(0);

  auto *i32_ty = llvm::Type::getInt32Ty(ctx.context);
  auto *ptr_ty = llvm::PointerType::getUnqual(ctx.context);
  auto *u32list_llvm_type = ctx.complex_type.u32list_type;

  FunctionSignature group_count_sign("GroupCount", {ValueType::kU32List}, ValueType::kU32);
  auto *group_count_func_type = llvm::FunctionType::get(i32_ty, {u32list_llvm_type}, false);
  llvm::FunctionCallee group_count_callee =
      ctx.module.getOrInsertFunction(group_count_sign.ToString(), group_count_func_type);
  llvm::Value *distinct_value = ctx.builder.CreateCall(group_count_callee, {group_index_value}, "group_count");

  FunctionSignature kernel_sign(kernel_name,
                                {sign.GetParamTypes().at(0), ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                                sign.GetRetType());
  auto *kernel_func_type =
      llvm::FunctionType::get(values_llvm_type, {values_llvm_type, u32list_llvm_type, i32_ty, ptr_ty}, false);
  llvm::FunctionCallee kernel_callee = ctx.module.getOrInsertFunction(kernel_sign.ToString(), kernel_func_type);
  return ctx.builder.CreateCall(kernel_callee, {values_value, group_index_value, distinct_value, exec_context_value},
                                call_name);
}

llvm::Value *EmitGroupMaxSugar(const FunctionSignature &sign, const std::vector<llvm::Type *> &arg_llvm_type_list,
                               const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  return EmitGroupExtremaSugar("GroupMax", sign, arg_llvm_type_list, arg_llvm_value_list, ctx, "group_max");
}

llvm::Value *EmitGroupMinSugar(const FunctionSignature &sign, const std::vector<llvm::Type *> &arg_llvm_type_list,
                               const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  return EmitGroupExtremaSugar("GroupMin", sign, arg_llvm_type_list, arg_llvm_value_list, ctx, "group_min");
}

Status InitGroupMaxSugarFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMax", {ValueType::kU8List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU8List),
      EmitGroupMaxSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMax", {ValueType::kI8List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI8List),
      EmitGroupMaxSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMax", {ValueType::kU16List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU16List),
      EmitGroupMaxSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMax", {ValueType::kI16List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI16List),
      EmitGroupMaxSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMax", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU32List),
      EmitGroupMaxSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMax", {ValueType::kI32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI32List),
      EmitGroupMaxSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMax", {ValueType::kU64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU64List),
      EmitGroupMaxSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMax", {ValueType::kI64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI64List),
      EmitGroupMaxSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMax", {ValueType::kF32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF32List),
      EmitGroupMaxSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMax", {ValueType::kF64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupMaxSugar));
  return Status::OK();
}

Status InitGroupMinSugarFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMin", {ValueType::kU8List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU8List),
      EmitGroupMinSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMin", {ValueType::kI8List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI8List),
      EmitGroupMinSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMin", {ValueType::kU16List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU16List),
      EmitGroupMinSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMin", {ValueType::kI16List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI16List),
      EmitGroupMinSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMin", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU32List),
      EmitGroupMinSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMin", {ValueType::kI32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI32List),
      EmitGroupMinSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMin", {ValueType::kU64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU64List),
      EmitGroupMinSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMin", {ValueType::kI64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kI64List),
      EmitGroupMinSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMin", {ValueType::kF32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF32List),
      EmitGroupMinSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupMin", {ValueType::kF64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupMinSugar));
  return Status::OK();
}

Status InitGroupAvgFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupAvg", {ValueType::kU8List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupAvg<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupAvg", {ValueType::kI8List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupAvg<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupAvg", {ValueType::kU16List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupAvg<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupAvg", {ValueType::kI16List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupAvg<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupAvg", {ValueType::kU32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupAvg<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupAvg", {ValueType::kI32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupAvg<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupAvg", {ValueType::kU64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupAvg<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupAvg", {ValueType::kI64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupAvg<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupAvg", {ValueType::kF32List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupAvg<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GroupAvg", {ValueType::kF64List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(GroupAvg<F64ListStruct>)));
  return Status::OK();
}

llvm::Value *EmitGroupAvgSugar(const FunctionSignature &sign, const std::vector<llvm::Type *> &arg_llvm_type_list,
                               const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *values_value = arg_llvm_value_list.at(0);
  auto *group_index_value = arg_llvm_value_list.at(1);
  auto *exec_context_value = arg_llvm_value_list.at(2);
  auto *values_llvm_type = arg_llvm_type_list.at(0);

  auto *i32_ty = llvm::Type::getInt32Ty(ctx.context);
  auto *ptr_ty = llvm::PointerType::getUnqual(ctx.context);
  auto *u32list_llvm_type = ctx.complex_type.u32list_type;

  llvm::Type *acc_llvm_type = nullptr;
  (void)CodeGen::ValueTypeToLLVMType(ctx, sign.GetRetType(), &acc_llvm_type);

  FunctionSignature group_count_sign("GroupCount", {ValueType::kU32List}, ValueType::kU32);
  auto *group_count_func_type = llvm::FunctionType::get(i32_ty, {u32list_llvm_type}, false);
  llvm::FunctionCallee group_count_callee =
      ctx.module.getOrInsertFunction(group_count_sign.ToString(), group_count_func_type);
  llvm::Value *distinct_value = ctx.builder.CreateCall(group_count_callee, {group_index_value}, "group_count");

  FunctionSignature group_avg_sign("GroupAvg",
                                   {sign.GetParamTypes().at(0), ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                                   sign.GetRetType());
  auto *group_avg_func_type =
      llvm::FunctionType::get(acc_llvm_type, {values_llvm_type, u32list_llvm_type, i32_ty, ptr_ty}, false);
  llvm::FunctionCallee group_avg_callee =
      ctx.module.getOrInsertFunction(group_avg_sign.ToString(), group_avg_func_type);
  return ctx.builder.CreateCall(group_avg_callee, {values_value, group_index_value, distinct_value, exec_context_value},
                                "group_avg");
}

Status InitGroupAvgSugarFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupAvg", {ValueType::kU8List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupAvgSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupAvg", {ValueType::kI8List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupAvgSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupAvg", {ValueType::kU16List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupAvgSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupAvg", {ValueType::kI16List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupAvgSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupAvg", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupAvgSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupAvg", {ValueType::kI32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupAvgSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupAvg", {ValueType::kU64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupAvgSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupAvg", {ValueType::kI64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupAvgSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupAvg", {ValueType::kF32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupAvgSugar));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("GroupAvg", {ValueType::kF64List, ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      EmitGroupAvgSugar));
  return Status::OK();
}

}  // namespace

Status InitListGroupFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(InitGroupIndexFunc(reg));
  JF_RETURN_NOT_OK(InitGroupCountFunc(reg));
  JF_RETURN_NOT_OK(InitGroupKeysFunc(reg));
  JF_RETURN_NOT_OK(InitGroupKeysSugarFunc(reg));
  JF_RETURN_NOT_OK(InitGroupSumFunc(reg));
  JF_RETURN_NOT_OK(InitGroupSumSugarFunc(reg));
  JF_RETURN_NOT_OK(InitGroupMaxFunc(reg));
  JF_RETURN_NOT_OK(InitGroupMaxSugarFunc(reg));
  JF_RETURN_NOT_OK(InitGroupMinFunc(reg));
  JF_RETURN_NOT_OK(InitGroupMinSugarFunc(reg));
  JF_RETURN_NOT_OK(InitGroupAvgFunc(reg));
  JF_RETURN_NOT_OK(InitGroupAvgSugarFunc(reg));
  return Status::OK();
}

}  // namespace jitfusion

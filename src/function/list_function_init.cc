/*
 * @Author: victorika
 * @Date: 2025-01-23 12:39:23
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-29 22:44:35
 */
#include <algorithm>
#include <unordered_set>
#include "exec_engine.h"
#include "function_init.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {

template <typename T>
LLVMComplexStruct ListConcat(LLVMComplexStruct a, LLVMComplexStruct b, int64_t exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  LLVMComplexStruct result;
  result.data = reinterpret_cast<int64_t>(exec_ctx->arena.Allocate((a.len + b.len) * sizeof(T)));
  result.len = a.len + b.len;
  memcpy(reinterpret_cast<T *>(result.data), reinterpret_cast<T *>(a.data), a.len * sizeof(T));
  memcpy(reinterpret_cast<T *>(result.data) + a.len, reinterpret_cast<T *>(b.data), b.len * sizeof(T));
  return result;
}

template <typename T>
uint8_t IsInList(T a, LLVMComplexStruct b) {
  if constexpr (std::is_same_v<float, T>) {
    for (size_t i = 0; i < b.len; ++i) {
      if (std::abs(a - reinterpret_cast<T *>(b.data)[i]) < std::numeric_limits<float>::epsilon()) {
        return 1;
      }
    }
    return 0;
  } else if constexpr (std::is_same_v<double, T>) {
    for (size_t i = 0; i < b.len; ++i) {
      if (std::abs(a - reinterpret_cast<T *>(b.data)[i]) < std::numeric_limits<double>::epsilon()) {
        return 1;
      }
    }
    return 0;
  } else {
    for (size_t i = 0; i < b.len; ++i) {
      if (a == reinterpret_cast<T *>(b.data)[i]) {
        return 1;
      }
    }
    return 0;
  }
}

inline uint8_t IsInStringList(LLVMComplexStruct a, LLVMComplexStruct b) {
  for (size_t i = 0; i < b.len; ++i) {
    if (a.len == reinterpret_cast<LLVMComplexStruct *>(b.data)[i].len &&
        strcmp(reinterpret_cast<char *>(a.data),
               reinterpret_cast<char *>(reinterpret_cast<LLVMComplexStruct *>(b.data)[i].data)) == 0) {
      return static_cast<uint8_t>(1);
    }
  }
  return static_cast<uint8_t>(0);
}

llvm::Value *CallBuiltinLenFunction(const FunctionSignature & /*sign*/,
                                    const std::vector<llvm::Type *> & /*arg_llvm_type_list*/,
                                    const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  llvm::Value *len_value = ctx.builder.CreateExtractValue(arg_llvm_value_list[0], {1}, "len_value");
  return len_value;
}

template <typename Ret, typename DType>
inline Ret Sum(LLVMComplexStruct a) {
  Ret sum = 0;
  for (size_t i = 0; i < a.len; ++i) {
    sum += static_cast<Ret>(reinterpret_cast<DType *>(a.data)[i]);
  }
  return sum;
}

template <typename T>
inline T Max(LLVMComplexStruct a) {
  return *std::max_element(reinterpret_cast<T *>(a.data), reinterpret_cast<T *>(a.data) + a.len);
}

template <typename T>
inline T Min(LLVMComplexStruct a) {
  return *std::min_element(reinterpret_cast<T *>(a.data), reinterpret_cast<T *>(a.data) + a.len);
}

template <typename T>
inline uint32_t CountDistinct(LLVMComplexStruct a) {
  std::unordered_set<T> unique_set(reinterpret_cast<T *>(a.data), reinterpret_cast<T *>(a.data) + a.len);
  return unique_set.size();
}

template <typename T>
inline LLVMComplexStruct SortAsc(LLVMComplexStruct a, int64_t exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  LLVMComplexStruct result;
  result.data = reinterpret_cast<int64_t>(exec_ctx->arena.Allocate((a.len) * sizeof(T)));
  result.len = a.len;
  memcpy(reinterpret_cast<T *>(result.data), reinterpret_cast<T *>(a.data), a.len * sizeof(T));
  std::sort(reinterpret_cast<T *>(result.data), reinterpret_cast<T *>(result.data) + result.len);
  return result;
}

template <typename T>
inline LLVMComplexStruct SortDesc(LLVMComplexStruct a, int64_t exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  LLVMComplexStruct result;
  result.data = reinterpret_cast<int64_t>(exec_ctx->arena.Allocate((a.len) * sizeof(T)));
  result.len = a.len;
  memcpy(reinterpret_cast<T *>(result.data), reinterpret_cast<T *>(a.data), a.len * sizeof(T));
  std::sort(reinterpret_cast<T *>(result.data), reinterpret_cast<T *>(result.data) + result.len, std::greater<T>());
  return result;
}

llvm::Value *CallBuiltinTruncateFunction(const FunctionSignature & /*sign*/,
                                         const std::vector<llvm::Type *> & /*arg_llvm_type_list*/,
                                         const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  llvm::Value *result_ptr = ctx.builder.CreateAlloca(ctx.complex_type, nullptr, "create result ptr");
  ctx.builder.CreateStore(arg_llvm_value_list[0], result_ptr);
  llvm::Value *len_ptr = ctx.builder.CreateGEP(ctx.complex_type, result_ptr,
                                               {llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx.context), 0),
                                                llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx.context), 1)},
                                               "load len_ptr");

  llvm::Value *new_len = arg_llvm_value_list[1];
  ctx.builder.CreateStore(new_len, len_ptr);
  llvm::Value *result = ctx.builder.CreateLoad(ctx.complex_type, result_ptr, "result");
  return result;
}

Status InitListConcatFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kU8List, ValueType::kU8List, ValueType::kI64}, ValueType::kU8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListConcat<uint8_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kU16List, ValueType::kU16List, ValueType::kI64}, ValueType::kU16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListConcat<uint16_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kU32List, ValueType::kU32List, ValueType::kI64}, ValueType::kU32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListConcat<uint32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kU64List, ValueType::kU64List, ValueType::kI64}, ValueType::kU64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListConcat<uint64_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kI8List, ValueType::kI8List, ValueType::kI64}, ValueType::kI8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListConcat<int8_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kI16List, ValueType::kI16List, ValueType::kI64}, ValueType::kI16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListConcat<int16_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kI32List, ValueType::kI32List, ValueType::kI64}, ValueType::kI32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListConcat<int32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kI64List, ValueType::kI64List, ValueType::kI64}, ValueType::kI64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListConcat<int64_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kF32List, ValueType::kF32List, ValueType::kI64}, ValueType::kF32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListConcat<float>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kF64List, ValueType::kF64List, ValueType::kI64}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListConcat<double>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kStringList, ValueType::kStringList, ValueType::kI64},
                        ValueType::kStringList),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListConcat<LLVMComplexStruct>), nullptr}));
  return Status::OK();
}

Status InitInFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("in", {ValueType::kU8, ValueType::kU8List}, ValueType::kU8),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<uint8_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("in", {ValueType::kU16, ValueType::kU16List}, ValueType::kU8),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<uint16_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("in", {ValueType::kU32, ValueType::kU32List}, ValueType::kU8),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<uint32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("in", {ValueType::kU64, ValueType::kU64List}, ValueType::kU8),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<uint64_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("in", {ValueType::kI8, ValueType::kI8List}, ValueType::kU8),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<int8_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("in", {ValueType::kI16, ValueType::kI16List}, ValueType::kU8),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<int16_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("in", {ValueType::kI32, ValueType::kI32List}, ValueType::kU8),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<int32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("in", {ValueType::kI64, ValueType::kI64List}, ValueType::kU8),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<int64_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("in", {ValueType::kF32, ValueType::kF32List}, ValueType::kU8),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<float>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("in", {ValueType::kF64, ValueType::kF64List}, ValueType::kU8),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<double>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("in", {ValueType::kString, ValueType::kStringList}, ValueType::kU8),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(IsInStringList), nullptr}));
  return Status::OK();
}

Status InitLenFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kU8List}, ValueType::kU32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLenFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kU16List}, ValueType::kU32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLenFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kU32List}, ValueType::kU32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLenFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kU64List}, ValueType::kU32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLenFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kI8List}, ValueType::kU32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLenFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kI16List}, ValueType::kU32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLenFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kI32List}, ValueType::kU32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLenFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kI64List}, ValueType::kU32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLenFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kF32List}, ValueType::kU32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLenFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kF64List}, ValueType::kU32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLenFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kStringList}, ValueType::kU32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLenFunction}));
  return Status::OK();
}

Status InitSumFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("Sum", {ValueType::kU8List}, ValueType::kU64),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<uint64_t, uint8_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Sum", {ValueType::kI8List}, ValueType::kI64),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<int64_t, int8_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("Sum", {ValueType::kU16List}, ValueType::kU64),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<uint64_t, uint16_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Sum", {ValueType::kI16List}, ValueType::kI64),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<int64_t, int16_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("Sum", {ValueType::kU32List}, ValueType::kU64),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<uint64_t, uint32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Sum", {ValueType::kI32List}, ValueType::kI64),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<int64_t, int32_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("Sum", {ValueType::kU64List}, ValueType::kU64),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<uint64_t, uint64_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Sum", {ValueType::kI64List}, ValueType::kI64),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<int64_t, int64_t>), nullptr}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Sum", {ValueType::kF32List}, ValueType::kF64),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<double, float>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Sum", {ValueType::kF64List}, ValueType::kF64),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<double, double>), nullptr}));
  return Status::OK();
}

Status InitMaxFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Max", {ValueType::kU8List}, ValueType::kU8),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Max<uint8_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Max", {ValueType::kI8List}, ValueType::kI8),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Max<int8_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Max", {ValueType::kU16List}, ValueType::kU16),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Max<uint16_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Max", {ValueType::kI16List}, ValueType::kI16),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Max<int16_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Max", {ValueType::kU32List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Max<uint32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Max", {ValueType::kI32List}, ValueType::kI32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Max<int32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Max", {ValueType::kU64List}, ValueType::kU64),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Max<uint64_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Max", {ValueType::kI64List}, ValueType::kI64),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Max<int64_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Max", {ValueType::kF32List}, ValueType::kF32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Max<float>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Max", {ValueType::kF64List}, ValueType::kF64),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Max<double>), nullptr}));
  return Status::OK();
}

Status InitMinFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Min", {ValueType::kU8List}, ValueType::kU8),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Min<uint8_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Min", {ValueType::kI8List}, ValueType::kI8),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Min<int8_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Min", {ValueType::kU16List}, ValueType::kU16),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Min<uint16_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Min", {ValueType::kI16List}, ValueType::kI16),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Min<int16_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Min", {ValueType::kU32List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Min<uint32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Min", {ValueType::kI32List}, ValueType::kI32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Min<int32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Min", {ValueType::kU64List}, ValueType::kU64),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Min<uint64_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Min", {ValueType::kI64List}, ValueType::kI64),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Min<int64_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Min", {ValueType::kF32List}, ValueType::kF32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Min<float>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Min", {ValueType::kF64List}, ValueType::kF64),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Min<double>), nullptr}));
  return Status::OK();
}

Status InitCountDistinctFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kU8List}, ValueType::kU32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<uint8_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kI8List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<int8_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kU16List}, ValueType::kU32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<uint16_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kI16List}, ValueType::kU32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<int16_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kU32List}, ValueType::kU32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<uint32_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kI32List}, ValueType::kU32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<int32_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kU64List}, ValueType::kU32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<uint64_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kI64List}, ValueType::kU32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<int64_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kF32List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<float>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kF64List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<double>), nullptr}));
  return Status::OK();
}

Status InitSortFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortAsc", {ValueType::kU8List, ValueType::kI64}, ValueType::kU8List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<uint8_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortAsc", {ValueType::kU16List, ValueType::kI64}, ValueType::kU16List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<uint16_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortAsc", {ValueType::kU32List, ValueType::kI64}, ValueType::kU32List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<uint32_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortAsc", {ValueType::kU64List, ValueType::kI64}, ValueType::kU64List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<uint64_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortAsc", {ValueType::kI8List, ValueType::kI64}, ValueType::kI8List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<int8_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortAsc", {ValueType::kI16List, ValueType::kI64}, ValueType::kI16List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<int16_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortAsc", {ValueType::kI32List, ValueType::kI64}, ValueType::kI32List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<int32_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortAsc", {ValueType::kI64List, ValueType::kI64}, ValueType::kI64List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<int64_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortAsc", {ValueType::kF32List, ValueType::kI64}, ValueType::kF32List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<float>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortAsc", {ValueType::kF64List, ValueType::kI64}, ValueType::kF64List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<double>), nullptr}));

  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortDesc", {ValueType::kU8List, ValueType::kI64}, ValueType::kU8List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<uint8_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortDesc", {ValueType::kU16List, ValueType::kI64}, ValueType::kU16List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<uint16_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortDesc", {ValueType::kU32List, ValueType::kI64}, ValueType::kU32List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<uint32_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortDesc", {ValueType::kU64List, ValueType::kI64}, ValueType::kU64List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<uint64_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortDesc", {ValueType::kI8List, ValueType::kI64}, ValueType::kI8List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<int8_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortDesc", {ValueType::kI16List, ValueType::kI64}, ValueType::kI16List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<int16_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortDesc", {ValueType::kI32List, ValueType::kI64}, ValueType::kI32List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<int32_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortDesc", {ValueType::kI64List, ValueType::kI64}, ValueType::kI64List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<int64_t>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortDesc", {ValueType::kF32List, ValueType::kI64}, ValueType::kF32List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<float>), nullptr}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("SortDesc", {ValueType::kF64List, ValueType::kI64}, ValueType::kF64List),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<double>), nullptr}));
  return Status::OK();
}

Status InitTruncateFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("Truncate", {ValueType::kU8List, ValueType::kU32}, ValueType::kU8List),
                        {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTruncateFunction}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("Truncate", {ValueType::kU16List, ValueType::kU32}, ValueType::kU16List),
                        {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTruncateFunction}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("Truncate", {ValueType::kU32List, ValueType::kU32}, ValueType::kU32List),
                        {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTruncateFunction}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("Truncate", {ValueType::kU64List, ValueType::kU32}, ValueType::kU64List),
                        {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTruncateFunction}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("Truncate", {ValueType::kI8List, ValueType::kU32}, ValueType::kI8List),
                        {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTruncateFunction}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("Truncate", {ValueType::kI16List, ValueType::kU32}, ValueType::kI16List),
                        {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTruncateFunction}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("Truncate", {ValueType::kI32List, ValueType::kU32}, ValueType::kI32List),
                        {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTruncateFunction}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("Truncate", {ValueType::kI64List, ValueType::kU32}, ValueType::kI64List),
                        {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTruncateFunction}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("Truncate", {ValueType::kF32List, ValueType::kU32}, ValueType::kF32List),
                        {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTruncateFunction}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("Truncate", {ValueType::kF64List, ValueType::kU32}, ValueType::kF64List),
                        {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTruncateFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Truncate", {ValueType::kStringList, ValueType::kU32}, ValueType::kStringList),
      {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTruncateFunction}));
  return Status::OK();
}

}  // namespace

Status InitListInternalFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(InitListConcatFunc(reg));
  JF_RETURN_NOT_OK(InitInFunc(reg));
  JF_RETURN_NOT_OK(InitLenFunc(reg));
  JF_RETURN_NOT_OK(InitSumFunc(reg));
  JF_RETURN_NOT_OK(InitMaxFunc(reg));
  JF_RETURN_NOT_OK(InitMinFunc(reg));
  JF_RETURN_NOT_OK(InitCountDistinctFunc(reg));
  JF_RETURN_NOT_OK(InitSortFunc(reg));
  JF_RETURN_NOT_OK(InitTruncateFunc(reg));
  return Status::OK();
}

}  // namespace jitfusion
/*
 * @Author: victorika
 * @Date: 2025-01-23 12:39:23
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-29 22:44:35
 */
#include <algorithm>
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

inline uint32_t Len(LLVMComplexStruct a) { return a.len; }

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
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Len), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kU16List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Len), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kU32List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Len), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kU64List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Len), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kI8List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Len), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kI16List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Len), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kI32List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Len), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kI64List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Len), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kF32List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Len), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kF64List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Len), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("Len", {ValueType::kStringList}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(Len), nullptr}));
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

}  // namespace

Status InitListInternalFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(InitListConcatFunc(reg));
  JF_RETURN_NOT_OK(InitInFunc(reg));
  JF_RETURN_NOT_OK(InitLenFunc(reg));
  JF_RETURN_NOT_OK(InitSumFunc(reg));
  JF_RETURN_NOT_OK(InitMaxFunc(reg));
  return Status::OK();
}

}  // namespace jitfusion
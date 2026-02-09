/*
 * @Author: victorika
 * @Date: 2026-02-09 15:01:48
 * @Last Modified by: victorika
 * @Last Modified time: 2026-02-09 15:05:28
 */
#include <algorithm>
#include <cstring>
#include <unordered_set>
#include "function_init.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {

template <typename RetType, typename ListType>
inline RetType Sum(ListType a) {
  RetType sum = 0;
  for (std::size_t i = 0; i < a.len; ++i) {
    sum += a.data[i];
  }
  return sum;
}

template <typename ListType>
inline typename ListType::CElementType Max(ListType a) {
  return *std::max_element(a.data, a.data + a.len);
}

template <typename ListType>
inline typename ListType::CElementType Min(ListType a) {
  return *std::min_element(a.data, a.data + a.len);
}

template <typename ListType>
inline uint32_t CountDistinct(ListType a) {
  std::unordered_set<typename ListType::CElementType> unique_set(a.data, a.data + a.len);
  return unique_set.size();
}

std::array<uint8_t, 256> poptable = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2,
    3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3,
    3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5,
    6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4,
    3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4,
    5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6,
    6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

uint32_t CountBits(U8ListStruct a) {
  uint32_t cnt = 0;
  for (uint32_t i = 0; i < a.len; i++) {
    cnt += poptable[a.data[i]];
  }
  return cnt;
}

Status InitSumFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Sum", {ValueType::kU8List}, ValueType::kU64),
                                              reinterpret_cast<void *>(Sum<uint64_t, U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Sum", {ValueType::kI8List}, ValueType::kI64),
                                              reinterpret_cast<void *>(Sum<int64_t, I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Sum", {ValueType::kU16List}, ValueType::kU64),
                                              reinterpret_cast<void *>(Sum<uint64_t, U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Sum", {ValueType::kI16List}, ValueType::kI64),
                                              reinterpret_cast<void *>(Sum<int64_t, I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Sum", {ValueType::kU32List}, ValueType::kU64),
                                              reinterpret_cast<void *>(Sum<uint64_t, U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Sum", {ValueType::kI32List}, ValueType::kI64),
                                              reinterpret_cast<void *>(Sum<int64_t, I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Sum", {ValueType::kU64List}, ValueType::kU64),
                                              reinterpret_cast<void *>(Sum<uint64_t, U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Sum", {ValueType::kI64List}, ValueType::kI64),
                                              reinterpret_cast<void *>(Sum<int64_t, I64ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Sum", {ValueType::kF32List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Sum<double, F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Sum", {ValueType::kF64List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Sum<double, F64ListStruct>)));
  return Status::OK();
}

Status InitMaxFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Max", {ValueType::kU8List}, ValueType::kU8),
                                              reinterpret_cast<void *>(Max<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Max", {ValueType::kI8List}, ValueType::kI8),
                                              reinterpret_cast<void *>(Max<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Max", {ValueType::kU16List}, ValueType::kU16),
                                              reinterpret_cast<void *>(Max<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Max", {ValueType::kI16List}, ValueType::kI16),
                                              reinterpret_cast<void *>(Max<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Max", {ValueType::kU32List}, ValueType::kU32),
                                              reinterpret_cast<void *>(Max<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Max", {ValueType::kI32List}, ValueType::kI32),
                                              reinterpret_cast<void *>(Max<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Max", {ValueType::kU64List}, ValueType::kU64),
                                              reinterpret_cast<void *>(Max<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Max", {ValueType::kI64List}, ValueType::kI64),
                                              reinterpret_cast<void *>(Max<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Max", {ValueType::kF32List}, ValueType::kF32),
                                              reinterpret_cast<void *>(Max<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Max", {ValueType::kF64List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Max<F64ListStruct>)));
  return Status::OK();
}

Status InitMinFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Min", {ValueType::kU8List}, ValueType::kU8),
                                              reinterpret_cast<void *>(Min<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Min", {ValueType::kI8List}, ValueType::kI8),
                                              reinterpret_cast<void *>(Min<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Min", {ValueType::kU16List}, ValueType::kU16),
                                              reinterpret_cast<void *>(Min<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Min", {ValueType::kI16List}, ValueType::kI16),
                                              reinterpret_cast<void *>(Min<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Min", {ValueType::kU32List}, ValueType::kU32),
                                              reinterpret_cast<void *>(Min<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Min", {ValueType::kI32List}, ValueType::kI32),
                                              reinterpret_cast<void *>(Min<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Min", {ValueType::kU64List}, ValueType::kU64),
                                              reinterpret_cast<void *>(Min<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Min", {ValueType::kI64List}, ValueType::kI64),
                                              reinterpret_cast<void *>(Min<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Min", {ValueType::kF32List}, ValueType::kF32),
                                              reinterpret_cast<void *>(Min<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Min", {ValueType::kF64List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Min<F64ListStruct>)));
  return Status::OK();
}

Status InitCountDistinctFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("CountDistinct", {ValueType::kU8List}, ValueType::kU32),
                                              reinterpret_cast<void *>(CountDistinct<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("CountDistinct", {ValueType::kI8List}, ValueType::kU32),
                                              reinterpret_cast<void *>(CountDistinct<I8ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("CountDistinct", {ValueType::kU16List}, ValueType::kU32),
                                 reinterpret_cast<void *>(CountDistinct<U16ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("CountDistinct", {ValueType::kI16List}, ValueType::kU32),
                                 reinterpret_cast<void *>(CountDistinct<I16ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("CountDistinct", {ValueType::kU32List}, ValueType::kU32),
                                 reinterpret_cast<void *>(CountDistinct<U32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("CountDistinct", {ValueType::kI32List}, ValueType::kU32),
                                 reinterpret_cast<void *>(CountDistinct<I32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("CountDistinct", {ValueType::kU64List}, ValueType::kU32),
                                 reinterpret_cast<void *>(CountDistinct<U64ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("CountDistinct", {ValueType::kI64List}, ValueType::kU32),
                                 reinterpret_cast<void *>(CountDistinct<I64ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("CountDistinct", {ValueType::kF32List}, ValueType::kU32),
                                 reinterpret_cast<void *>(CountDistinct<F32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("CountDistinct", {ValueType::kF64List}, ValueType::kU32),
                                 reinterpret_cast<void *>(CountDistinct<F64ListStruct>)));
  return Status::OK();
}

Status InitCountBitsFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("CountBits", {ValueType::kU8List}, ValueType::kU32),
                                              reinterpret_cast<void *>(CountBits)));
  return Status::OK();
}

}  // namespace

Status InitListAggregationFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(InitSumFunc(reg));
  JF_RETURN_NOT_OK(InitMaxFunc(reg));
  JF_RETURN_NOT_OK(InitMinFunc(reg));
  JF_RETURN_NOT_OK(InitCountDistinctFunc(reg));
  JF_RETURN_NOT_OK(InitCountBitsFunc(reg));
  return Status::OK();
}

}  // namespace jitfusion
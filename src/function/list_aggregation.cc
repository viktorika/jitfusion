/*
 * @Author: victorika
 * @Date: 2026-02-09 15:01:48
 * @Last Modified by: victorika
 * @Last Modified time: 2026-02-09 15:05:28
 */
#include <algorithm>
#include <cstring>
#include <unordered_set>
#include <vector>
#ifdef _MSC_VER
#  include <intrin.h>
#endif
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
inline double Avg(ListType a) {
  if (a.len == 0) {
    return 0.0;
  }
  double sum = 0.0;
  for (std::size_t i = 0; i < a.len; ++i) {
    sum += static_cast<double>(a.data[i]);
  }
  return sum / static_cast<double>(a.len);
}

template <typename ListType>
inline typename ListType::CElementType Max(ListType a) {
  if (a.len == 0) {
    return typename ListType::CElementType{};
  }
  return *std::max_element(a.data, a.data + a.len);
}

template <typename ListType>
inline typename ListType::CElementType Min(ListType a) {
  if (a.len == 0) {
    return typename ListType::CElementType{};
  }
  return *std::min_element(a.data, a.data + a.len);
}

template <typename ListType>
inline uint32_t CountDistinct(ListType a) {
  std::unordered_set<typename ListType::CElementType> unique_set(a.data, a.data + a.len);
  return unique_set.size();
}

inline uint64_t Popcountll(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
  return static_cast<uint64_t>(__builtin_popcountll(x));
#elif defined(_MSC_VER)
  return __popcnt64(x);
#else
  x = x - ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  return (x * 0x0101010101010101ULL) >> 56;
#endif
}

inline uint32_t Popcount(uint32_t x) {
#if defined(__GNUC__) || defined(__clang__)
  return static_cast<uint32_t>(__builtin_popcount(x));
#elif defined(_MSC_VER)
  return __popcnt(x);
#else
  x = x - ((x >> 1) & 0x55555555U);
  x = (x & 0x33333333U) + ((x >> 2) & 0x33333333U);
  x = (x + (x >> 4)) & 0x0F0F0F0FU;
  return (x * 0x01010101U) >> 24;
#endif
}

uint32_t CountBits(U8ListStruct a) {
  uint32_t cnt = 0;
  uint32_t i = 0;

  uint32_t batch_end = a.len & ~7U;
  for (; i < batch_end; i += 8) {
    uint64_t val;
    memcpy(&val, a.data + i, 8);
    cnt += static_cast<uint32_t>(Popcountll(val));
  }

  for (; i < a.len; i++) {
    cnt += Popcount(a.data[i]);
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

template <typename ListType>
inline double Median(ListType a) {
  if (a.len == 0) {
    return 0.0;
  }
  std::vector<typename ListType::CElementType> tmp(a.data, a.data + a.len);
  auto mid = a.len / 2;
  std::nth_element(tmp.begin(), tmp.begin() + mid, tmp.end());
  if (a.len % 2 == 1) {
    return static_cast<double>(tmp[mid]);
  }
  auto left_max = *std::max_element(tmp.begin(), tmp.begin() + mid);
  return (static_cast<double>(left_max) + static_cast<double>(tmp[mid])) / 2.0;
}

template <typename ListType>
inline double SortedMedian(ListType a) {
  if (a.len == 0) {
    return 0.0;
  }
  auto mid = a.len / 2;
  if (a.len % 2 == 1) {
    return static_cast<double>(a.data[mid]);
  }
  return (static_cast<double>(a.data[mid - 1]) + static_cast<double>(a.data[mid])) / 2.0;
}

Status InitMedianFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Median", {ValueType::kU8List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Median<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Median", {ValueType::kI8List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Median<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Median", {ValueType::kU16List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Median<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Median", {ValueType::kI16List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Median<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Median", {ValueType::kU32List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Median<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Median", {ValueType::kI32List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Median<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Median", {ValueType::kU64List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Median<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Median", {ValueType::kI64List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Median<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Median", {ValueType::kF32List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Median<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Median", {ValueType::kF64List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Median<F64ListStruct>)));
  return Status::OK();
}

Status InitSortedMedianFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("SortedMedian", {ValueType::kU8List}, ValueType::kF64),
                                              reinterpret_cast<void *>(SortedMedian<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("SortedMedian", {ValueType::kI8List}, ValueType::kF64),
                                              reinterpret_cast<void *>(SortedMedian<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("SortedMedian", {ValueType::kU16List}, ValueType::kF64),
                                              reinterpret_cast<void *>(SortedMedian<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("SortedMedian", {ValueType::kI16List}, ValueType::kF64),
                                              reinterpret_cast<void *>(SortedMedian<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("SortedMedian", {ValueType::kU32List}, ValueType::kF64),
                                              reinterpret_cast<void *>(SortedMedian<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("SortedMedian", {ValueType::kI32List}, ValueType::kF64),
                                              reinterpret_cast<void *>(SortedMedian<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("SortedMedian", {ValueType::kU64List}, ValueType::kF64),
                                              reinterpret_cast<void *>(SortedMedian<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("SortedMedian", {ValueType::kI64List}, ValueType::kF64),
                                              reinterpret_cast<void *>(SortedMedian<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("SortedMedian", {ValueType::kF32List}, ValueType::kF64),
                                              reinterpret_cast<void *>(SortedMedian<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("SortedMedian", {ValueType::kF64List}, ValueType::kF64),
                                              reinterpret_cast<void *>(SortedMedian<F64ListStruct>)));
  return Status::OK();
}

template <typename ListType>
inline typename ListType::CElementType GetAt(ListType a, uint32_t index) {
  if (index >= a.len) {
    return typename ListType::CElementType{};
  }
  return a.data[index];
}

Status InitGetAtFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("GetAt", {ValueType::kU8List, ValueType::kU32}, ValueType::kU8),
                                 reinterpret_cast<void *>(GetAt<U8ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("GetAt", {ValueType::kI8List, ValueType::kU32}, ValueType::kI8),
                                 reinterpret_cast<void *>(GetAt<I8ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("GetAt", {ValueType::kU16List, ValueType::kU32}, ValueType::kU16),
                                 reinterpret_cast<void *>(GetAt<U16ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("GetAt", {ValueType::kI16List, ValueType::kU32}, ValueType::kI16),
                                 reinterpret_cast<void *>(GetAt<I16ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("GetAt", {ValueType::kU32List, ValueType::kU32}, ValueType::kU32),
                                 reinterpret_cast<void *>(GetAt<U32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("GetAt", {ValueType::kI32List, ValueType::kU32}, ValueType::kI32),
                                 reinterpret_cast<void *>(GetAt<I32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("GetAt", {ValueType::kU64List, ValueType::kU32}, ValueType::kU64),
                                 reinterpret_cast<void *>(GetAt<U64ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("GetAt", {ValueType::kI64List, ValueType::kU32}, ValueType::kI64),
                                 reinterpret_cast<void *>(GetAt<I64ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("GetAt", {ValueType::kF32List, ValueType::kU32}, ValueType::kF32),
                                 reinterpret_cast<void *>(GetAt<F32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("GetAt", {ValueType::kF64List, ValueType::kU32}, ValueType::kF64),
                                 reinterpret_cast<void *>(GetAt<F64ListStruct>)));
  return Status::OK();
}

Status InitAvgFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Avg", {ValueType::kU8List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Avg<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Avg", {ValueType::kI8List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Avg<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Avg", {ValueType::kU16List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Avg<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Avg", {ValueType::kI16List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Avg<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Avg", {ValueType::kU32List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Avg<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Avg", {ValueType::kI32List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Avg<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Avg", {ValueType::kU64List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Avg<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Avg", {ValueType::kI64List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Avg<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Avg", {ValueType::kF32List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Avg<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("Avg", {ValueType::kF64List}, ValueType::kF64),
                                              reinterpret_cast<void *>(Avg<F64ListStruct>)));
  return Status::OK();
}

}  // namespace

Status InitListAggregationFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(InitSumFunc(reg));
  JF_RETURN_NOT_OK(InitMaxFunc(reg));
  JF_RETURN_NOT_OK(InitMinFunc(reg));
  JF_RETURN_NOT_OK(InitCountDistinctFunc(reg));
  JF_RETURN_NOT_OK(InitCountBitsFunc(reg));
  JF_RETURN_NOT_OK(InitGetAtFunc(reg));
  JF_RETURN_NOT_OK(InitAvgFunc(reg));
  JF_RETURN_NOT_OK(InitMedianFunc(reg));
  JF_RETURN_NOT_OK(InitSortedMedianFunc(reg));
  return Status::OK();
}

}  // namespace jitfusion
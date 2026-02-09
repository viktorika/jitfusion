/*
 * @Author: victorika
 * @Date: 2026-02-09 14:48:23
 * @Last Modified by: victorika
 * @Last Modified time: 2026-02-09 14:58:26
 */
#include <cstring>
#include "exec_engine.h"
#include "function_init.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {

template <typename ListType>
ListType IfLarge(ListType a, typename ListType::CElementType cmp_value, typename ListType::CElementType target_value,
                 void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  for (std::size_t i = 0; i < a.len; i++) {
    result.data[i] = a.data[i] > cmp_value ? target_value : a.data[i];
  }
  return result;
}

template <typename ListType>
ListType IfLargeEqual(ListType a, typename ListType::CElementType cmp_value,
                      typename ListType::CElementType target_value, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  for (std::size_t i = 0; i < a.len; i++) {
    result.data[i] = a.data[i] >= cmp_value ? target_value : a.data[i];
  }
  return result;
}

template <typename ListType>
ListType IfEqual(ListType a, typename ListType::CElementType cmp_value, typename ListType::CElementType target_value,
                 void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  for (std::size_t i = 0; i < a.len; i++) {
    result.data[i] = a.data[i] == cmp_value ? target_value : a.data[i];
  }
  return result;
}

template <typename ListType>
ListType IfLess(ListType a, typename ListType::CElementType cmp_value, typename ListType::CElementType target_value,
                void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  for (std::size_t i = 0; i < a.len; i++) {
    result.data[i] = a.data[i] < cmp_value ? target_value : a.data[i];
  }
  return result;
}

template <typename ListType>
ListType IfLessEqual(ListType a, typename ListType::CElementType cmp_value,
                     typename ListType::CElementType target_value, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  for (std::size_t i = 0; i < a.len; i++) {
    result.data[i] = a.data[i] <= cmp_value ? target_value : a.data[i];
  }
  return result;
}

template <typename ListType>
ListType IfNotEqual(ListType a, typename ListType::CElementType cmp_value, typename ListType::CElementType target_value,
                    void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  for (std::size_t i = 0; i < a.len; i++) {
    result.data[i] = a.data[i] != cmp_value ? target_value : a.data[i];
  }
  return result;
}

template <typename ListType>
ListType IfByBitmapLLRB(U8ListStruct bitmap, ListType lhs, typename ListType::CElementType rhs, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((lhs.len) * sizeof(typename ListType::CElementType)));
  result.len = lhs.len;
  uint32_t loop_len = result.len & (~7U);
  uint32_t i = 0;
  for (; i < loop_len; i += 8) {
    int byte_index = i / 8;
    for (int j = 0; j < 8; j++) {
      result.data[i + j] = (bitmap.data[byte_index] >> j) & 1;
    }
  }
  auto last_index = (loop_len / 8);
  for (int j = 0; i + j < result.len; j++) {
    result.data[i + j] = (bitmap.data[last_index] >> j) & 1;
  }
  for (uint32_t j = 0; j < result.len; j++) {
    typename ListType::CElementType mask = (result.data[j] > 0);
    result.data[j] = lhs.data[j] * mask + rhs * (1 - mask);
  }
  return result;
}

template <typename ListType>
ListType IfByBitmapLBRL(U8ListStruct bitmap, typename ListType::CElementType lhs, ListType rhs, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((rhs.len) * sizeof(typename ListType::CElementType)));
  result.len = rhs.len;
  uint32_t loop_len = result.len & (~7U);
  uint32_t i = 0;
  for (; i < loop_len; i += 8) {
    int byte_index = i / 8;
    for (int j = 0; j < 8; j++) {
      result.data[i + j] = (bitmap.data[byte_index] >> j) & 1;
    }
  }
  auto last_index = (loop_len / 8);
  for (int j = 0; i + j < result.len; j++) {
    result.data[i + j] = (bitmap.data[last_index] >> j) & 1;
  }
  for (uint32_t j = 0; j < result.len; j++) {
    typename ListType::CElementType mask = (result.data[j] > 0);
    result.data[j] = lhs * mask + rhs.data[j] * (1 - mask);
  }
  return result;
}

template <typename ListType>
ListType IfByBitmapLLRL(U8ListStruct bitmap, ListType lhs, ListType rhs, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((lhs.len) * sizeof(typename ListType::CElementType)));
  result.len = lhs.len;
  uint32_t loop_len = result.len & (~7U);
  uint32_t i = 0;
  for (; i < loop_len; i += 8) {
    int byte_index = i / 8;
    for (int j = 0; j < 8; j++) {
      result.data[i + j] = (bitmap.data[byte_index] >> j) & 1;
    }
  }
  auto last_index = (loop_len / 8);
  for (int j = 0; i + j < result.len; j++) {
    result.data[i + j] = (bitmap.data[last_index] >> j) & 1;
  }
  for (uint32_t j = 0; j < result.len; j++) {
    typename ListType::CElementType mask = (result.data[j] > 0);
    result.data[j] = lhs.data[j] * mask + rhs.data[j] * (1 - mask);
  }
  return result;
}

template <typename ListType, BinaryOPType OpType>
U8ListStruct GenBitmap(ListType a, typename ListType::CElementType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  U8ListStruct result;
  result.len = (a.len + 7) / 8;
  result.data = reinterpret_cast<U8ListStruct::CElementType *>(
      exec_ctx->arena.Allocate(result.len * sizeof(U8ListStruct::CElementType)));
  uint32_t vec_loop_len = a.len & (~7U);
  uint32_t i = 0;
  for (; i < vec_loop_len; i += 8) {
    uint8_t mask_bit = 0;
    for (int j = 0; j < 8; ++j) {
      uint32_t is_true;
      if constexpr (BinaryOPType::kLarge == OpType) {
        is_true = (a.data[i + j] > b ? 1U : 0U);
      } else if constexpr (BinaryOPType::kLargeEqual == OpType) {
        is_true = (a.data[i + j] >= b ? 1U : 0U);
      } else if constexpr (BinaryOPType::kEqual == OpType) {
        is_true = (a.data[i + j] == b ? 1U : 0U);
      } else if constexpr (BinaryOPType::kLess == OpType) {
        is_true = (a.data[i + j] < b ? 1U : 0U);
      } else if constexpr (BinaryOPType::kLessEqual == OpType) {
        is_true = (a.data[i + j] <= b ? 1U : 0U);
      } else if constexpr (BinaryOPType::kNotEqual == OpType) {
        is_true = (a.data[i + j] != b ? 1U : 0U);
      }
      mask_bit |= (is_true << j);
    }
    result.data[(i / 8)] = mask_bit;
  }
  auto last_index = (vec_loop_len / 8);
  result.data[last_index] = 0;
  for (int j = 0; i + j < a.len; j++) {
    uint32_t is_true;
    if constexpr (BinaryOPType::kLarge == OpType) {
      is_true = (a.data[i + j] > b ? 1U : 0U);
    } else if constexpr (BinaryOPType::kLargeEqual == OpType) {
      is_true = (a.data[i + j] >= b ? 1U : 0U);
    } else if constexpr (BinaryOPType::kEqual == OpType) {
      is_true = (a.data[i + j] == b ? 1U : 0U);
    } else if constexpr (BinaryOPType::kLess == OpType) {
      is_true = (a.data[i + j] < b ? 1U : 0U);
    } else if constexpr (BinaryOPType::kLessEqual == OpType) {
      is_true = (a.data[i + j] <= b ? 1U : 0U);
    } else if constexpr (BinaryOPType::kNotEqual == OpType) {
      is_true = (a.data[i + j] != b ? 1U : 0U);
    }
    result.data[last_index] |= (is_true << j);
  }
  return result;
}

template <typename ListType, BinaryOPType OpType>
U8ListStruct GenBitmapWithMinSize(ListType a, ListType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  auto len = std::min(a.len, b.len);
  U8ListStruct result;
  result.len = (len + 7) / 8;
  result.data = reinterpret_cast<U8ListStruct::CElementType *>(
      exec_ctx->arena.Allocate(result.len * sizeof(U8ListStruct::CElementType)));
  uint32_t vec_loop_len = len & (~7U);
  uint32_t i = 0;
  for (; i < vec_loop_len; i += 8) {
    uint8_t mask_bit = 0;
    for (int j = 0; j < 8; ++j) {
      uint32_t is_true;
      if constexpr (BinaryOPType::kLarge == OpType) {
        is_true = (a.data[i + j] > b.data[i + j] ? 1U : 0U);
      } else if constexpr (BinaryOPType::kLargeEqual == OpType) {
        is_true = (a.data[i + j] >= b.data[i + j] ? 1U : 0U);
      } else if constexpr (BinaryOPType::kEqual == OpType) {
        is_true = (a.data[i + j] == b.data[i + j] ? 1U : 0U);
      } else if constexpr (BinaryOPType::kLess == OpType) {
        is_true = (a.data[i + j] < b.data[i + j] ? 1U : 0U);
      } else if constexpr (BinaryOPType::kLessEqual == OpType) {
        is_true = (a.data[i + j] <= b.data[i + j] ? 1U : 0U);
      } else if constexpr (BinaryOPType::kNotEqual == OpType) {
        is_true = (a.data[i + j] != b.data[i + j] ? 1U : 0U);
      }
      mask_bit |= (is_true << j);
    }
    result.data[(i / 8)] = mask_bit;
  }
  auto last_index = (vec_loop_len / 8);
  result.data[last_index] = 0;
  for (int j = 0; i + j < len; j++) {
    uint32_t is_true;
    if constexpr (BinaryOPType::kLarge == OpType) {
      is_true = (a.data[i + j] > b.data[i + j] ? 1U : 0U);
    } else if constexpr (BinaryOPType::kLargeEqual == OpType) {
      is_true = (a.data[i + j] >= b.data[i + j] ? 1U : 0U);
    } else if constexpr (BinaryOPType::kEqual == OpType) {
      is_true = (a.data[i + j] == b.data[i + j] ? 1U : 0U);
    } else if constexpr (BinaryOPType::kLess == OpType) {
      is_true = (a.data[i + j] < b.data[i + j] ? 1U : 0U);
    } else if constexpr (BinaryOPType::kLessEqual == OpType) {
      is_true = (a.data[i + j] <= b.data[i + j] ? 1U : 0U);
    } else if constexpr (BinaryOPType::kNotEqual == OpType) {
      is_true = (a.data[i + j] != b.data[i + j] ? 1U : 0U);
    }
    result.data[last_index] |= (is_true << j);
  }
  return result;
}

std::array<std::array<int8_t, 8>, 256> filter_index_table;
int GenerateFilterIndexTable() {
  for (int mask = 0; mask < 256; mask++) {
    int idx = 0;
    for (int j = 0; j < 8; j++) {
      if ((mask & (1 << j)) != 0) {
        filter_index_table[mask][idx++] = j;
      }
    }
    for (; idx < 8; idx++) {
      filter_index_table[mask][idx] = -1;
    }
  }
  return 0;
}
auto gen_filter_index_table_ret = GenerateFilterIndexTable();

template <typename ListType>
ListType FilterByBitmap(ListType a, U8ListStruct bitmap, uint32_t bits_cnt, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate(bits_cnt * sizeof(typename ListType::CElementType)));
  result.len = bits_cnt;
  uint32_t cur = 0;
  for (std::size_t i = 0; i < bitmap.len; i++) {
    uint8_t mask = bitmap.data[i];
    const auto &indices = filter_index_table[mask];
    for (auto idx : indices) {
      if (-1 == idx) {
        break;
      }
      result.data[cur++] = (a.data[(i * 8) + idx]);
    }
  }
  return result;
}

Status InitIfLargeFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLarge", {ValueType::kU8List, ValueType::kU8, ValueType::kU8, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(IfLarge<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLarge", {ValueType::kU16List, ValueType::kU16, ValueType::kU16, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(IfLarge<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLarge", {ValueType::kU32List, ValueType::kU32, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(IfLarge<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLarge", {ValueType::kU64List, ValueType::kU64, ValueType::kU64, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(IfLarge<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLarge", {ValueType::kI8List, ValueType::kI8, ValueType::kI8, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(IfLarge<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLarge", {ValueType::kI16List, ValueType::kI16, ValueType::kI16, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(IfLarge<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLarge", {ValueType::kI32List, ValueType::kI32, ValueType::kI32, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(IfLarge<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLarge", {ValueType::kI64List, ValueType::kI64, ValueType::kI64, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(IfLarge<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLarge", {ValueType::kF32List, ValueType::kF32, ValueType::kF32, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(IfLarge<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLarge", {ValueType::kF64List, ValueType::kF64, ValueType::kF64, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(IfLarge<F64ListStruct>)));
  return Status::OK();
}

Status InitIfLargeEqualFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLargeEqual", {ValueType::kU8List, ValueType::kU8, ValueType::kU8, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(IfLargeEqual<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLargeEqual", {ValueType::kU16List, ValueType::kU16, ValueType::kU16, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(IfLargeEqual<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLargeEqual", {ValueType::kU32List, ValueType::kU32, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(IfLargeEqual<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLargeEqual", {ValueType::kU64List, ValueType::kU64, ValueType::kU64, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(IfLargeEqual<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLargeEqual", {ValueType::kI8List, ValueType::kI8, ValueType::kI8, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(IfLargeEqual<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLargeEqual", {ValueType::kI16List, ValueType::kI16, ValueType::kI16, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(IfLargeEqual<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLargeEqual", {ValueType::kI32List, ValueType::kI32, ValueType::kI32, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(IfLargeEqual<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLargeEqual", {ValueType::kI64List, ValueType::kI64, ValueType::kI64, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(IfLargeEqual<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLargeEqual", {ValueType::kF32List, ValueType::kF32, ValueType::kF32, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(IfLargeEqual<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLargeEqual", {ValueType::kF64List, ValueType::kF64, ValueType::kF64, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(IfLargeEqual<F64ListStruct>)));
  return Status::OK();
}

Status InitIfEqualFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfEqual", {ValueType::kU8List, ValueType::kU8, ValueType::kU8, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(IfEqual<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfEqual", {ValueType::kU16List, ValueType::kU16, ValueType::kU16, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(IfEqual<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfEqual", {ValueType::kU32List, ValueType::kU32, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(IfEqual<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfEqual", {ValueType::kU64List, ValueType::kU64, ValueType::kU64, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(IfEqual<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfEqual", {ValueType::kI8List, ValueType::kI8, ValueType::kI8, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(IfEqual<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfEqual", {ValueType::kI16List, ValueType::kI16, ValueType::kI16, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(IfEqual<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfEqual", {ValueType::kI32List, ValueType::kI32, ValueType::kI32, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(IfEqual<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfEqual", {ValueType::kI64List, ValueType::kI64, ValueType::kI64, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(IfEqual<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfEqual", {ValueType::kF32List, ValueType::kF32, ValueType::kF32, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(IfEqual<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfEqual", {ValueType::kF64List, ValueType::kF64, ValueType::kF64, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(IfEqual<F64ListStruct>)));
  return Status::OK();
}

Status InitIfLessFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLess", {ValueType::kU8List, ValueType::kU8, ValueType::kU8, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(IfLess<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLess", {ValueType::kU16List, ValueType::kU16, ValueType::kU16, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(IfLess<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLess", {ValueType::kU32List, ValueType::kU32, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(IfLess<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLess", {ValueType::kU64List, ValueType::kU64, ValueType::kU64, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(IfLess<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLess", {ValueType::kI8List, ValueType::kI8, ValueType::kI8, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(IfLess<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLess", {ValueType::kI16List, ValueType::kI16, ValueType::kI16, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(IfLess<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLess", {ValueType::kI32List, ValueType::kI32, ValueType::kI32, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(IfLess<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLess", {ValueType::kI64List, ValueType::kI64, ValueType::kI64, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(IfLess<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLess", {ValueType::kF32List, ValueType::kF32, ValueType::kF32, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(IfLess<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLess", {ValueType::kF64List, ValueType::kF64, ValueType::kF64, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(IfLess<F64ListStruct>)));
  return Status::OK();
}

Status InitIfLessEqualFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLessEqual", {ValueType::kU8List, ValueType::kU8, ValueType::kU8, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(IfLessEqual<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLessEqual", {ValueType::kU16List, ValueType::kU16, ValueType::kU16, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(IfLessEqual<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLessEqual", {ValueType::kU32List, ValueType::kU32, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(IfLessEqual<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLessEqual", {ValueType::kU64List, ValueType::kU64, ValueType::kU64, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(IfLessEqual<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLessEqual", {ValueType::kI8List, ValueType::kI8, ValueType::kI8, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(IfLessEqual<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLessEqual", {ValueType::kI16List, ValueType::kI16, ValueType::kI16, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(IfLessEqual<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLessEqual", {ValueType::kI32List, ValueType::kI32, ValueType::kI32, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(IfLessEqual<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLessEqual", {ValueType::kI64List, ValueType::kI64, ValueType::kI64, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(IfLessEqual<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLessEqual", {ValueType::kF32List, ValueType::kF32, ValueType::kF32, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(IfLessEqual<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfLessEqual", {ValueType::kF64List, ValueType::kF64, ValueType::kF64, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(IfLessEqual<F64ListStruct>)));
  return Status::OK();
}

Status InitIfNotEqualFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfNotEqual", {ValueType::kU8List, ValueType::kU8, ValueType::kU8, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(IfNotEqual<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfNotEqual", {ValueType::kU16List, ValueType::kU16, ValueType::kU16, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(IfNotEqual<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfNotEqual", {ValueType::kU32List, ValueType::kU32, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(IfNotEqual<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfNotEqual", {ValueType::kU64List, ValueType::kU64, ValueType::kU64, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(IfNotEqual<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfNotEqual", {ValueType::kI8List, ValueType::kI8, ValueType::kI8, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(IfNotEqual<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfNotEqual", {ValueType::kI16List, ValueType::kI16, ValueType::kI16, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(IfNotEqual<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfNotEqual", {ValueType::kI32List, ValueType::kI32, ValueType::kI32, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(IfNotEqual<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfNotEqual", {ValueType::kI64List, ValueType::kI64, ValueType::kI64, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(IfNotEqual<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfNotEqual", {ValueType::kF32List, ValueType::kF32, ValueType::kF32, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(IfNotEqual<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfNotEqual", {ValueType::kF64List, ValueType::kF64, ValueType::kF64, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(IfNotEqual<F64ListStruct>)));
  return Status::OK();
}

Status InitIfByBitmapLLRB(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kU8List, ValueType::kU8, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(IfByBitmapLLRB<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kU16List, ValueType::kU16, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(IfByBitmapLLRB<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(IfByBitmapLLRB<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kU64List, ValueType::kU64, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(IfByBitmapLLRB<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kI8List, ValueType::kI8, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(IfByBitmapLLRB<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kI16List, ValueType::kI16, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(IfByBitmapLLRB<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kI32List, ValueType::kI32, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(IfByBitmapLLRB<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kI64List, ValueType::kI64, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(IfByBitmapLLRB<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kF32List, ValueType::kF32, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(IfByBitmapLLRB<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kF64List, ValueType::kF64, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(IfByBitmapLLRB<F64ListStruct>)));
  return Status::OK();
}

Status InitIfByBitmapLBRL(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kU8, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(IfByBitmapLBRL<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kU16, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(IfByBitmapLBRL<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kU32, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(IfByBitmapLBRL<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kU64, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(IfByBitmapLBRL<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kI8, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(IfByBitmapLBRL<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kI16, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(IfByBitmapLBRL<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kI32, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(IfByBitmapLBRL<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kI64, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(IfByBitmapLBRL<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kF32, ValueType::kF32List, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(IfByBitmapLBRL<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kF64, ValueType::kF64List, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(IfByBitmapLBRL<F64ListStruct>)));
  return Status::OK();
}

Status InitIfByBitmapLLRL(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(IfByBitmapLLRL<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(IfByBitmapLLRL<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(IfByBitmapLLRL<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(IfByBitmapLLRL<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(IfByBitmapLLRL<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(IfByBitmapLLRL<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(IfByBitmapLLRL<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(IfByBitmapLLRL<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kF32List, ValueType::kF32List, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(IfByBitmapLLRL<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("IfByBitmap", {ValueType::kU8List, ValueType::kF64List, ValueType::kF64List, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(IfByBitmapLLRL<F64ListStruct>)));
  return Status::OK();
}

Status InitGenLargeBitmapFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmap", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U8ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmap", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U16ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmap", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U32ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmap", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U64ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmap", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I8ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmap", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I16ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmap", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I32ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmap", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I64ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmap", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<F32ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmap", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<F64ListStruct, BinaryOPType::kLarge>)));
  return Status::OK();
}

Status InitGenLargeBitmapWithMinSizeFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmapWithMinSize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U8ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmapWithMinSize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U16ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmapWithMinSize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U32ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmapWithMinSize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U64ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmapWithMinSize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I8ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmapWithMinSize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I16ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmapWithMinSize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I32ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmapWithMinSize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I64ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmapWithMinSize", {ValueType::kF32List, ValueType::kF32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<F32ListStruct, BinaryOPType::kLarge>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeBitmapWithMinSize", {ValueType::kF64List, ValueType::kF64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<F64ListStruct, BinaryOPType::kLarge>)));
  return Status::OK();
}

Status InitGenLargeEqualBitmapFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmap", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U8ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmap", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U16ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmap", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U32ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmap", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U64ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmap", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I8ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmap", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I16ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmap", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I32ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmap", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I64ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmap", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<F32ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmap", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<F64ListStruct, BinaryOPType::kLargeEqual>)));
  return Status::OK();
}

Status InitGenLargeEqualBitmapWithMinSizeFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmapWithMinSize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U8ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmapWithMinSize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U16ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmapWithMinSize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U32ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmapWithMinSize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U64ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmapWithMinSize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I8ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmapWithMinSize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I16ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmapWithMinSize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I32ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmapWithMinSize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I64ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmapWithMinSize", {ValueType::kF32List, ValueType::kF32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<F32ListStruct, BinaryOPType::kLargeEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLargeEqualBitmapWithMinSize", {ValueType::kF64List, ValueType::kF64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<F64ListStruct, BinaryOPType::kLargeEqual>)));
  return Status::OK();
}

Status InitGenEqualBitmapFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmap", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U8ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmap", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U16ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmap", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U32ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmap", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U64ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmap", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I8ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmap", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I16ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmap", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I32ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmap", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I64ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmap", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<F32ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmap", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<F64ListStruct, BinaryOPType::kEqual>)));
  return Status::OK();
}

Status InitGenEqualBitmapWithMinSizeFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmapWithMinSize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U8ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmapWithMinSize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U16ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmapWithMinSize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U32ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmapWithMinSize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U64ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmapWithMinSize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I8ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmapWithMinSize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I16ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmapWithMinSize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I32ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmapWithMinSize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I64ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmapWithMinSize", {ValueType::kF32List, ValueType::kF32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<F32ListStruct, BinaryOPType::kEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenEqualBitmapWithMinSize", {ValueType::kF64List, ValueType::kF64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<F64ListStruct, BinaryOPType::kEqual>)));
  return Status::OK();
}

Status InitGenLessBitmapFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmap", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U8ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmap", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U16ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmap", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U32ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmap", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U64ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmap", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I8ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmap", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I16ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmap", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I32ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmap", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I64ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmap", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<F32ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmap", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<F64ListStruct, BinaryOPType::kLess>)));
  return Status::OK();
}

Status InitGenLessBitmapWithMinSizeFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmapWithMinSize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U8ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmapWithMinSize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U16ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmapWithMinSize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U32ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmapWithMinSize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U64ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmapWithMinSize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I8ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmapWithMinSize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I16ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmapWithMinSize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I32ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmapWithMinSize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I64ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmapWithMinSize", {ValueType::kF32List, ValueType::kF32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<F32ListStruct, BinaryOPType::kLess>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessBitmapWithMinSize", {ValueType::kF64List, ValueType::kF64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<F64ListStruct, BinaryOPType::kLess>)));
  return Status::OK();
}

Status InitGenLessEqualBitmapFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmap", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U8ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmap", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U16ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmap", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U32ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmap", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U64ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmap", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I8ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmap", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I16ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmap", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I32ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmap", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I64ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmap", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<F32ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmap", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<F64ListStruct, BinaryOPType::kLessEqual>)));
  return Status::OK();
}

Status InitGenLessEqualBitmapWithMinSizeFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmapWithMinSize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U8ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmapWithMinSize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U16ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmapWithMinSize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U32ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmapWithMinSize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U64ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmapWithMinSize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I8ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmapWithMinSize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I16ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmapWithMinSize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I32ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmapWithMinSize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I64ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmapWithMinSize", {ValueType::kF32List, ValueType::kF32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<F32ListStruct, BinaryOPType::kLessEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenLessEqualBitmapWithMinSize", {ValueType::kF64List, ValueType::kF64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<F64ListStruct, BinaryOPType::kLessEqual>)));
  return Status::OK();
}

Status InitGenNotEqualBitmapFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmap", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U8ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmap", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U16ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmap", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U32ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmap", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<U64ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmap", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I8ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmap", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I16ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmap", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I32ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmap", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<I64ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmap", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<F32ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmap", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmap<F64ListStruct, BinaryOPType::kNotEqual>)));
  return Status::OK();
}

Status InitGenNotEqualBitmapWithMinSizeFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmapWithMinSize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U8ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmapWithMinSize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U16ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmapWithMinSize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U32ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmapWithMinSize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<U64ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmapWithMinSize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I8ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmapWithMinSize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I16ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmapWithMinSize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I32ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmapWithMinSize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<I64ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmapWithMinSize", {ValueType::kF32List, ValueType::kF32List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<F32ListStruct, BinaryOPType::kNotEqual>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("GenNotEqualBitmapWithMinSize", {ValueType::kF64List, ValueType::kF64List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(GenBitmapWithMinSize<F64ListStruct, BinaryOPType::kNotEqual>)));
  return Status::OK();
}

Status InitFilterByBitmapFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FilterByBitmap", {ValueType::kU8List, ValueType::kU8List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(FilterByBitmap<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FilterByBitmap", {ValueType::kU16List, ValueType::kU8List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(FilterByBitmap<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FilterByBitmap", {ValueType::kU32List, ValueType::kU8List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(FilterByBitmap<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FilterByBitmap", {ValueType::kU64List, ValueType::kU8List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(FilterByBitmap<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FilterByBitmap", {ValueType::kI8List, ValueType::kU8List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(FilterByBitmap<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FilterByBitmap", {ValueType::kI16List, ValueType::kU8List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(FilterByBitmap<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FilterByBitmap", {ValueType::kI32List, ValueType::kU8List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(FilterByBitmap<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FilterByBitmap", {ValueType::kI64List, ValueType::kU8List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(FilterByBitmap<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FilterByBitmap", {ValueType::kF32List, ValueType::kU8List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(FilterByBitmap<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("FilterByBitmap", {ValueType::kF64List, ValueType::kU8List, ValueType::kU32, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(FilterByBitmap<F64ListStruct>)));
  return Status::OK();
}

Status InitIfFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(InitIfLargeFunc(reg));
  JF_RETURN_NOT_OK(InitIfLargeEqualFunc(reg));
  JF_RETURN_NOT_OK(InitIfEqualFunc(reg));
  JF_RETURN_NOT_OK(InitIfLessFunc(reg));
  JF_RETURN_NOT_OK(InitIfLessEqualFunc(reg));
  JF_RETURN_NOT_OK(InitIfNotEqualFunc(reg));
  JF_RETURN_NOT_OK(InitIfByBitmapLLRB(reg));
  JF_RETURN_NOT_OK(InitIfByBitmapLBRL(reg));
  JF_RETURN_NOT_OK(InitIfByBitmapLLRL(reg));
  return Status::OK();
}

Status InitFilterFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(InitGenLargeBitmapFunc(reg));
  JF_RETURN_NOT_OK(InitGenLargeBitmapWithMinSizeFunc(reg));
  JF_RETURN_NOT_OK(InitGenLargeEqualBitmapFunc(reg));
  JF_RETURN_NOT_OK(InitGenLargeEqualBitmapWithMinSizeFunc(reg));
  JF_RETURN_NOT_OK(InitGenEqualBitmapFunc(reg));
  JF_RETURN_NOT_OK(InitGenEqualBitmapWithMinSizeFunc(reg));
  JF_RETURN_NOT_OK(InitGenLessBitmapFunc(reg));
  JF_RETURN_NOT_OK(InitGenLessBitmapWithMinSizeFunc(reg));
  JF_RETURN_NOT_OK(InitGenLessEqualBitmapFunc(reg));
  JF_RETURN_NOT_OK(InitGenLessEqualBitmapWithMinSizeFunc(reg));
  JF_RETURN_NOT_OK(InitGenNotEqualBitmapFunc(reg));
  JF_RETURN_NOT_OK(InitGenNotEqualBitmapWithMinSizeFunc(reg));
  JF_RETURN_NOT_OK(InitFilterByBitmapFunc(reg));
  return Status::OK();
}

}  // namespace

Status InitListComparisonFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(InitIfFunc(reg));
  JF_RETURN_NOT_OK(InitFilterFunc(reg));
  return Status::OK();
}

}  // namespace jitfusion
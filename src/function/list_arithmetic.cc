/*
 * @Author: victorika
 * @Date: 2026-02-09 14:30:21
 * @Last Modified by: victorika
 * @Last Modified time: 2026-02-09 14:42:43
 */
#include <algorithm>
#include <cmath>
#include <cstring>
#include "exec_engine.h"
#include "function_init.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

#ifdef HAS_XSIMD
#  include <xsimd/xsimd.hpp>
namespace xs = xsimd;
#endif

namespace jitfusion {

namespace {

template <typename ListType>
ListType ListAdd(ListType a, typename ListType::CElementType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = a.data[i] + b;
  }
  return result;
}

template <typename ListType>
ListType ListAddWithMinSize(ListType a, ListType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.len = std::min(a.len, b.len);
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((result.len) * sizeof(typename ListType::CElementType)));
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ListType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = result.len - (result.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto b_vec = batch_type::load_unaligned(b.data + i);
    auto add_vec = a_vec + b_vec;
    add_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < result.len; ++i) {
    result.data[i] = a.data[i] + b.data[i];
  }
#else
  for (uint32_t i = 0; i < result.len; i++) {
    result.data[i] = a.data[i] + b.data[i];
  }
#endif
  return result;
}

template <typename ListType>
ListType ListSub(ListType a, typename ListType::CElementType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = a.data[i] - b;
  }
  return result;
}

template <typename ListType>
ListType ListSubWithMinSize(ListType a, ListType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.len = std::min(a.len, b.len);
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((result.len) * sizeof(typename ListType::CElementType)));
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ListType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = result.len - (result.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto b_vec = batch_type::load_unaligned(b.data + i);
    auto add_vec = a_vec - b_vec;
    add_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < result.len; ++i) {
    result.data[i] = a.data[i] - b.data[i];
  }
#else
  for (uint32_t i = 0; i < result.len; i++) {
    result.data[i] = a.data[i] - b.data[i];
  }
#endif
  return result;
}

template <typename ListType>
ListType ListMul(ListType a, typename ListType::CElementType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = a.data[i] * b;
  }
  return result;
}

template <typename ListType>
ListType ListMulWithMinSize(ListType a, ListType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.len = std::min(a.len, b.len);
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((result.len) * sizeof(typename ListType::CElementType)));
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ListType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = result.len - (result.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto b_vec = batch_type::load_unaligned(b.data + i);
    auto add_vec = a_vec * b_vec;
    add_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < result.len; ++i) {
    result.data[i] = a.data[i] * b.data[i];
  }
#else
  for (uint32_t i = 0; i < result.len; i++) {
    result.data[i] = a.data[i] * b.data[i];
  }
#endif
  return result;
}

template <typename ListType>
ListType ListDiv(ListType a, typename ListType::CElementType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = a.data[i] / b;
  }
  return result;
}

template <typename ListType>
ListType ListDivWithMinSize(ListType a, ListType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.len = std::min(a.len, b.len);
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((result.len) * sizeof(typename ListType::CElementType)));
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ListType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = result.len - (result.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto b_vec = batch_type::load_unaligned(b.data + i);
    auto add_vec = a_vec / b_vec;
    add_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < result.len; ++i) {
    result.data[i] = a.data[i] / b.data[i];
  }
#else
  for (uint32_t i = 0; i < result.len; i++) {
    result.data[i] = a.data[i] / b.data[i];
  }
#endif
  return result;
}

template <typename ListType>
ListType ListMod(ListType a, typename ListType::CElementType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = a.data[i] % b;
  }
  return result;
}

template <typename ListType>
ListType ListModWithMinSize(ListType a, ListType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.len = std::min(a.len, b.len);
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((result.len) * sizeof(typename ListType::CElementType)));
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ListType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = result.len - (result.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto b_vec = batch_type::load_unaligned(b.data + i);
    auto add_vec = a_vec % b_vec;
    add_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < result.len; ++i) {
    result.data[i] = a.data[i] % b.data[i];
  }
#else
  for (uint32_t i = 0; i < result.len; i++) {
    result.data[i] = a.data[i] % b.data[i];
  }
#endif
  return result;
}

template <typename ListType>
ListType ListBitwiseAnd(ListType a, typename ListType::CElementType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = a.data[i] & b;
  }
  return result;
}

template <typename ListType>
ListType ListBitwiseAndWithMinSize(ListType a, ListType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.len = std::min(a.len, b.len);
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((result.len) * sizeof(typename ListType::CElementType)));
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ListType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = result.len - (result.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto b_vec = batch_type::load_unaligned(b.data + i);
    auto add_vec = a_vec & b_vec;
    add_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < result.len; ++i) {
    result.data[i] = a.data[i] & b.data[i];
  }
#else
  for (uint32_t i = 0; i < result.len; i++) {
    result.data[i] = a.data[i] & b.data[i];
  }
#endif
  return result;
}

template <typename ListType>
ListType ListBitwiseOr(ListType a, typename ListType::CElementType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = a.data[i] | b;
  }
  return result;
}

template <typename ListType>
ListType ListBitwiseOrWithMinSize(ListType a, ListType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.len = std::min(a.len, b.len);
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((result.len) * sizeof(typename ListType::CElementType)));
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ListType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = result.len - (result.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto b_vec = batch_type::load_unaligned(b.data + i);
    auto add_vec = a_vec | b_vec;
    add_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < result.len; ++i) {
    result.data[i] = a.data[i] | b.data[i];
  }
#else
  for (uint32_t i = 0; i < result.len; i++) {
    result.data[i] = a.data[i] | b.data[i];
  }
#endif
  return result;
}

template <typename ListType>
ListType ListBitwiseXor(ListType a, typename ListType::CElementType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = a.data[i] ^ b;
  }
  return result;
}

template <typename ListType>
ListType ListBitwiseXorWithMinSize(ListType a, ListType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.len = std::min(a.len, b.len);
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((result.len) * sizeof(typename ListType::CElementType)));
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ListType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = result.len - (result.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto b_vec = batch_type::load_unaligned(b.data + i);
    auto add_vec = a_vec ^ b_vec;
    add_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < result.len; ++i) {
    result.data[i] = a.data[i] ^ b.data[i];
  }
#else
  for (uint32_t i = 0; i < result.len; i++) {
    result.data[i] = a.data[i] ^ b.data[i];
  }
#endif
  return result;
}

template <typename ListType, typename ResultType>
ResultType ListExp(ListType a, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ResultType result;
  result.data = reinterpret_cast<typename ResultType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ResultType::CElementType)));
  result.len = a.len;
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ResultType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = a.len - (a.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto exp_vec = xs::exp(a_vec);
    exp_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < a.len; ++i) {
    result.data[i] = std::exp(a.data[i]);
  }
#else
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = std::exp(a.data[i]);
  }
#endif
  return result;
}

template <typename ListType, typename ResultType>
ResultType ListLog(ListType a, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ResultType result;
  result.data = reinterpret_cast<typename ResultType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ResultType::CElementType)));
  result.len = a.len;
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ResultType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = a.len - (a.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto log_vec = xs::log(a_vec);
    log_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < a.len; ++i) {
    result.data[i] = std::log(a.data[i]);
  }
#else
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = std::log(a.data[i]);
  }
#endif
  return result;
}

template <typename ListType, typename ResultType>
ResultType ListLog2(ListType a, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ResultType result;
  result.data = reinterpret_cast<typename ResultType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ResultType::CElementType)));
  result.len = a.len;
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ResultType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = a.len - (a.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto log_vec = xs::log2(a_vec);
    log_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < a.len; ++i) {
    result.data[i] = std::log2(a.data[i]);
  }
#else
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = std::log2(a.data[i]);
  }
#endif
  return result;
}

template <typename ListType, typename ResultType>
ResultType ListLog10(ListType a, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ResultType result;
  result.data = reinterpret_cast<typename ResultType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ResultType::CElementType)));
  result.len = a.len;
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ResultType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = a.len - (a.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto log_vec = xs::log10(a_vec);
    log_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < a.len; ++i) {
    result.data[i] = std::log10(a.data[i]);
  }
#else
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = std::log10(a.data[i]);
  }
#endif
  return result;
}

template <typename ListType>
ListType ListCeil(ListType a, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ListType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = a.len - (a.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto log_vec = xs::ceil(a_vec);
    log_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < a.len; ++i) {
    result.data[i] = std::ceil(a.data[i]);
  }
#else
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = std::ceil(a.data[i]);
  }
#endif
  return result;
}

template <typename ListType>
ListType ListFloor(ListType a, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ListType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = a.len - (a.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto log_vec = xs::floor(a_vec);
    log_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < a.len; ++i) {
    result.data[i] = std::floor(a.data[i]);
  }
#else
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = std::floor(a.data[i]);
  }
#endif
  return result;
}

template <typename ListType>
ListType ListRound(ListType a, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ListType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = a.len - (a.len % kBatchSize);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto log_vec = xs::round(a_vec);
    log_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < a.len; ++i) {
    result.data[i] = std::round(a.data[i]);
  }
#else
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = std::round(a.data[i]);
  }
#endif
  return result;
}

template <typename ListType>
ListType ListMin(ListType a, typename ListType::CElementType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ListType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = a.len - (a.len % kBatchSize);
  auto b_vec = batch_type(b);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto log_vec = xs::min(a_vec, b_vec);
    log_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < a.len; ++i) {
    result.data[i] = std::min(a.data[i], b);
  }
#else
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = std::min(a.data[i], b);
  }
#endif
  return result;
}

template <typename ListType>
ListType ListMax(ListType a, typename ListType::CElementType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
#ifdef HAS_XSIMD
  using batch_type = xs::batch<typename ListType::CElementType, xs::default_arch>;
  constexpr std::size_t kBatchSize = batch_type::size;
  auto vec_size = a.len - (a.len % kBatchSize);
  auto b_vec = batch_type(b);
  for (std::size_t i = 0; i < vec_size; i += kBatchSize) {
    auto a_vec = batch_type::load_unaligned(a.data + i);
    auto log_vec = xs::max(a_vec, b_vec);
    log_vec.store_unaligned(result.data + i);
  }
  for (std::size_t i = vec_size; i < a.len; ++i) {
    result.data[i] = std::max(a.data[i], b);
  }
#else
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = std::max(a.data[i], b);
  }
#endif
  return result;
}

Status InitListAddFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListAdd", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(ListAdd<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListAdd", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      reinterpret_cast<void *>(ListAdd<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListAdd", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(ListAdd<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListAdd", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      reinterpret_cast<void *>(ListAdd<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListAdd", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      reinterpret_cast<void *>(ListAdd<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListAdd", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      reinterpret_cast<void *>(ListAdd<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListAdd", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      reinterpret_cast<void *>(ListAdd<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListAdd", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      reinterpret_cast<void *>(ListAdd<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListAdd", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(ListAdd<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListAdd", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListAdd<F64ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListAddWithMinSize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(ListAddWithMinSize<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListAddWithMinSize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(ListAddWithMinSize<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListAddWithMinSize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListAddWithMinSize<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListAddWithMinSize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(ListAddWithMinSize<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListAddWithMinSize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(ListAddWithMinSize<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListAddWithMinSize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(ListAddWithMinSize<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListAddWithMinSize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(ListAddWithMinSize<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListAddWithMinSize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(ListAddWithMinSize<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListAddWithMinSize", {ValueType::kF32List, ValueType::kF32List, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(ListAddWithMinSize<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListAddWithMinSize", {ValueType::kF64List, ValueType::kF64List, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(ListAddWithMinSize<F64ListStruct>)));
  return Status::OK();
}

Status InitListSubFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSub", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(ListSub<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSub", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      reinterpret_cast<void *>(ListSub<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSub", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(ListSub<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSub", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      reinterpret_cast<void *>(ListSub<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSub", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      reinterpret_cast<void *>(ListSub<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSub", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      reinterpret_cast<void *>(ListSub<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSub", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      reinterpret_cast<void *>(ListSub<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSub", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      reinterpret_cast<void *>(ListSub<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSub", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(ListSub<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSub", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListSub<F64ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSubWithMinSize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(ListSubWithMinSize<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSubWithMinSize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(ListSubWithMinSize<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSubWithMinSize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListSubWithMinSize<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSubWithMinSize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(ListSubWithMinSize<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSubWithMinSize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(ListSubWithMinSize<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSubWithMinSize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(ListSubWithMinSize<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSubWithMinSize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(ListSubWithMinSize<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSubWithMinSize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(ListSubWithMinSize<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSubWithMinSize", {ValueType::kF32List, ValueType::kF32List, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(ListSubWithMinSize<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListSubWithMinSize", {ValueType::kF64List, ValueType::kF64List, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(ListSubWithMinSize<F64ListStruct>)));
  return Status::OK();
}

Status InitListMulFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMul", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(ListMul<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMul", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      reinterpret_cast<void *>(ListMul<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMul", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(ListMul<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMul", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      reinterpret_cast<void *>(ListMul<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMul", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      reinterpret_cast<void *>(ListMul<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMul", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      reinterpret_cast<void *>(ListMul<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMul", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      reinterpret_cast<void *>(ListMul<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMul", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      reinterpret_cast<void *>(ListMul<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMul", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(ListMul<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMul", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListMul<F64ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListMulWithMinSize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(ListMulWithMinSize<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListMulWithMinSize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(ListMulWithMinSize<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListMulWithMinSize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListMulWithMinSize<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListMulWithMinSize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(ListMulWithMinSize<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListMulWithMinSize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(ListMulWithMinSize<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListMulWithMinSize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(ListMulWithMinSize<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListMulWithMinSize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(ListMulWithMinSize<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListMulWithMinSize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(ListMulWithMinSize<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListMulWithMinSize", {ValueType::kF32List, ValueType::kF32List, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(ListMulWithMinSize<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterCommutativeCFunc(
      FunctionSignature("ListMulWithMinSize", {ValueType::kF64List, ValueType::kF64List, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(ListMulWithMinSize<F64ListStruct>)));
  return Status::OK();
}

Status InitListDivFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDiv", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(ListDiv<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDiv", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      reinterpret_cast<void *>(ListDiv<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDiv", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(ListDiv<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDiv", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      reinterpret_cast<void *>(ListDiv<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDiv", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      reinterpret_cast<void *>(ListDiv<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDiv", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      reinterpret_cast<void *>(ListDiv<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDiv", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      reinterpret_cast<void *>(ListDiv<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDiv", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      reinterpret_cast<void *>(ListDiv<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDiv", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(ListDiv<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDiv", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListDiv<F64ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDivWithMinSize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(ListDivWithMinSize<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDivWithMinSize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(ListDivWithMinSize<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDivWithMinSize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListDivWithMinSize<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDivWithMinSize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(ListDivWithMinSize<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDivWithMinSize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(ListDivWithMinSize<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDivWithMinSize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(ListDivWithMinSize<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDivWithMinSize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(ListDivWithMinSize<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDivWithMinSize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(ListDivWithMinSize<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDivWithMinSize", {ValueType::kF32List, ValueType::kF32List, ValueType::kPtr},
                        ValueType::kF32List),
      reinterpret_cast<void *>(ListDivWithMinSize<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListDivWithMinSize", {ValueType::kF64List, ValueType::kF64List, ValueType::kPtr},
                        ValueType::kF64List),
      reinterpret_cast<void *>(ListDivWithMinSize<F64ListStruct>)));
  return Status::OK();
}

Status InitListModFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMod", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(ListMod<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMod", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      reinterpret_cast<void *>(ListMod<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMod", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(ListMod<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMod", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      reinterpret_cast<void *>(ListMod<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMod", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      reinterpret_cast<void *>(ListMod<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMod", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      reinterpret_cast<void *>(ListMod<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMod", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      reinterpret_cast<void *>(ListMod<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMod", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      reinterpret_cast<void *>(ListMod<I64ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListModWithMinSize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(ListModWithMinSize<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListModWithMinSize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(ListModWithMinSize<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListModWithMinSize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListModWithMinSize<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListModWithMinSize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(ListModWithMinSize<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListModWithMinSize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(ListModWithMinSize<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListModWithMinSize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(ListModWithMinSize<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListModWithMinSize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(ListModWithMinSize<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListModWithMinSize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(ListModWithMinSize<I64ListStruct>)));
  return Status::OK();
}

Status InitListBitwiseAndFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAnd", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(ListBitwiseAnd<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAnd", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      reinterpret_cast<void *>(ListBitwiseAnd<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAnd", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(ListBitwiseAnd<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAnd", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      reinterpret_cast<void *>(ListBitwiseAnd<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAnd", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      reinterpret_cast<void *>(ListBitwiseAnd<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAnd", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      reinterpret_cast<void *>(ListBitwiseAnd<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAnd", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      reinterpret_cast<void *>(ListBitwiseAnd<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAnd", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      reinterpret_cast<void *>(ListBitwiseAnd<I64ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAndWithMinSize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(ListBitwiseAndWithMinSize<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAndWithMinSize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(ListBitwiseAndWithMinSize<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAndWithMinSize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListBitwiseAndWithMinSize<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAndWithMinSize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(ListBitwiseAndWithMinSize<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAndWithMinSize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(ListBitwiseAndWithMinSize<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAndWithMinSize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(ListBitwiseAndWithMinSize<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAndWithMinSize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(ListBitwiseAndWithMinSize<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseAndWithMinSize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(ListBitwiseAndWithMinSize<I64ListStruct>)));
  return Status::OK();
}

Status InitListBitwiseOrFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOr", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(ListBitwiseOr<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOr", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      reinterpret_cast<void *>(ListBitwiseOr<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOr", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(ListBitwiseOr<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOr", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      reinterpret_cast<void *>(ListBitwiseOr<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOr", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      reinterpret_cast<void *>(ListBitwiseOr<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOr", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      reinterpret_cast<void *>(ListBitwiseOr<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOr", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      reinterpret_cast<void *>(ListBitwiseOr<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOr", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      reinterpret_cast<void *>(ListBitwiseOr<I64ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOrWithMinSize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(ListBitwiseOrWithMinSize<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOrWithMinSize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(ListBitwiseOrWithMinSize<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOrWithMinSize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListBitwiseOrWithMinSize<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOrWithMinSize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(ListBitwiseOrWithMinSize<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOrWithMinSize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(ListBitwiseOrWithMinSize<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOrWithMinSize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(ListBitwiseOrWithMinSize<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOrWithMinSize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(ListBitwiseOrWithMinSize<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseOrWithMinSize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(ListBitwiseOrWithMinSize<I64ListStruct>)));
  return Status::OK();
}

Status InitListBitwiseXorFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXor", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(ListBitwiseXor<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXor", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      reinterpret_cast<void *>(ListBitwiseXor<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXor", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(ListBitwiseXor<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXor", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      reinterpret_cast<void *>(ListBitwiseXor<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXor", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      reinterpret_cast<void *>(ListBitwiseXor<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXor", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      reinterpret_cast<void *>(ListBitwiseXor<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXor", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      reinterpret_cast<void *>(ListBitwiseXor<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXor", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      reinterpret_cast<void *>(ListBitwiseXor<I64ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXorWithMinSize", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr},
                        ValueType::kU8List),
      reinterpret_cast<void *>(ListBitwiseXorWithMinSize<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXorWithMinSize", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr},
                        ValueType::kU16List),
      reinterpret_cast<void *>(ListBitwiseXorWithMinSize<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXorWithMinSize", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr},
                        ValueType::kU32List),
      reinterpret_cast<void *>(ListBitwiseXorWithMinSize<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXorWithMinSize", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr},
                        ValueType::kU64List),
      reinterpret_cast<void *>(ListBitwiseXorWithMinSize<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXorWithMinSize", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr},
                        ValueType::kI8List),
      reinterpret_cast<void *>(ListBitwiseXorWithMinSize<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXorWithMinSize", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr},
                        ValueType::kI16List),
      reinterpret_cast<void *>(ListBitwiseXorWithMinSize<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXorWithMinSize", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr},
                        ValueType::kI32List),
      reinterpret_cast<void *>(ListBitwiseXorWithMinSize<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListBitwiseXorWithMinSize", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr},
                        ValueType::kI64List),
      reinterpret_cast<void *>(ListBitwiseXorWithMinSize<I64ListStruct>)));
  return Status::OK();
}

Status InitListExpFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListExp", {ValueType::kU8List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListExp<U8ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListExp", {ValueType::kU16List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListExp<U16ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListExp", {ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListExp<U32ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListExp", {ValueType::kU64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListExp<U64ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListExp", {ValueType::kI8List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListExp<I8ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListExp", {ValueType::kI16List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListExp<I16ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListExp", {ValueType::kI32List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListExp<I32ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListExp", {ValueType::kI64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListExp<I64ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListExp", {ValueType::kF32List, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(ListExp<F32ListStruct, F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListExp", {ValueType::kF64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListExp<F64ListStruct, F64ListStruct>)));
  return Status::OK();
}

Status InitListLogFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog", {ValueType::kU8List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog<U8ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog", {ValueType::kU16List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog<U16ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog", {ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog<U32ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog", {ValueType::kU64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog<U64ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog", {ValueType::kI8List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog<I8ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog", {ValueType::kI16List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog<I16ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog", {ValueType::kI32List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog<I32ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog", {ValueType::kI64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog<I64ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog", {ValueType::kF32List, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(ListLog<F32ListStruct, F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog", {ValueType::kF64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog<F64ListStruct, F64ListStruct>)));
  return Status::OK();
}

Status InitListLog2Func(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog2", {ValueType::kU8List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog2<U8ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog2", {ValueType::kU16List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog2<U16ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog2", {ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog2<U32ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog2", {ValueType::kU64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog2<U64ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog2", {ValueType::kI8List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog2<I8ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog2", {ValueType::kI16List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog2<I16ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog2", {ValueType::kI32List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog2<I32ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog2", {ValueType::kI64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog2<I64ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog2", {ValueType::kF32List, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(ListLog2<F32ListStruct, F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog2", {ValueType::kF64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog2<F64ListStruct, F64ListStruct>)));
  return Status::OK();
}

Status InitListLog10Func(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog10", {ValueType::kU8List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog10<U8ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog10", {ValueType::kU16List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog10<U16ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog10", {ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog10<U32ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog10", {ValueType::kU64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog10<U64ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog10", {ValueType::kI8List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog10<I8ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog10", {ValueType::kI16List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog10<I16ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog10", {ValueType::kI32List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog10<I32ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog10", {ValueType::kI64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog10<I64ListStruct, F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog10", {ValueType::kF32List, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(ListLog10<F32ListStruct, F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListLog10", {ValueType::kF64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListLog10<F64ListStruct, F64ListStruct>)));
  return Status::OK();
}

Status InitListCeilFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListCeil", {ValueType::kF32List, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(ListCeil<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListCeil", {ValueType::kF64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListCeil<F64ListStruct>)));
  return Status::OK();
}

Status InitListFloorFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListFloor", {ValueType::kF32List, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(ListFloor<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListFloor", {ValueType::kF64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListFloor<F64ListStruct>)));
  return Status::OK();
}

Status InitListRoundFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListRound", {ValueType::kF32List, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(ListRound<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListRound", {ValueType::kF64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListRound<F64ListStruct>)));
  return Status::OK();
}

Status InitListMinFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMin", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(ListMin<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMin", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      reinterpret_cast<void *>(ListMin<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMin", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(ListMin<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMin", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      reinterpret_cast<void *>(ListMin<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMin", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      reinterpret_cast<void *>(ListMin<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMin", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      reinterpret_cast<void *>(ListMin<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMin", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      reinterpret_cast<void *>(ListMin<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMin", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      reinterpret_cast<void *>(ListMin<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMin", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(ListMin<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMin", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListMin<F64ListStruct>)));
  return Status::OK();
}

Status InitListMaxFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMax", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(ListMax<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMax", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      reinterpret_cast<void *>(ListMax<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMax", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(ListMax<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMax", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      reinterpret_cast<void *>(ListMax<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMax", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      reinterpret_cast<void *>(ListMax<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMax", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      reinterpret_cast<void *>(ListMax<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMax", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      reinterpret_cast<void *>(ListMax<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMax", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      reinterpret_cast<void *>(ListMax<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMax", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(ListMax<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListMax", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListMax<F64ListStruct>)));
  return Status::OK();
}

}  // namespace

Status InitListArithmeticFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(InitListAddFunc(reg));
  JF_RETURN_NOT_OK(InitListSubFunc(reg));
  JF_RETURN_NOT_OK(InitListMulFunc(reg));
  JF_RETURN_NOT_OK(InitListDivFunc(reg));
  JF_RETURN_NOT_OK(InitListModFunc(reg));
  JF_RETURN_NOT_OK(InitListBitwiseAndFunc(reg));
  JF_RETURN_NOT_OK(InitListBitwiseOrFunc(reg));
  JF_RETURN_NOT_OK(InitListBitwiseXorFunc(reg));
  JF_RETURN_NOT_OK(InitListExpFunc(reg));
  JF_RETURN_NOT_OK(InitListLogFunc(reg));
  JF_RETURN_NOT_OK(InitListLog2Func(reg));
  JF_RETURN_NOT_OK(InitListLog10Func(reg));
  JF_RETURN_NOT_OK(InitListCeilFunc(reg));
  JF_RETURN_NOT_OK(InitListFloorFunc(reg));
  JF_RETURN_NOT_OK(InitListRoundFunc(reg));
  JF_RETURN_NOT_OK(InitListMinFunc(reg));
  JF_RETURN_NOT_OK(InitListMaxFunc(reg));
  return Status::OK();
}

}  // namespace jitfusion
/*
 * @Author: victorika
 * @Date: 2025-01-23 12:39:23
 * @Last Modified by: victorika
 * @Last Modified time: 2025-04-02 17:04:12
 */
#include <algorithm>
#include <cmath>
#include <cstring>
#include <string_view>
#include <unordered_set>
#include "codegen/codegen.h"
#include "exec_engine.h"
#include "function_init.h"
#include "function_registry.h"
#include "llvm/IR/Value.h"
#include "murmurhash3.h"
#include "status.h"
#include "type.h"

#ifdef HAS_XSIMD
#  include <xsimd/xsimd.hpp>
namespace xs = xsimd;
#endif

namespace jitfusion {

namespace {

template <typename ListType>
ListType ListConcat(ListType a, ListType b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.len = a.len + b.len;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate(result.len * sizeof(typename ListType::CElementType)));
  memcpy(result.data, a.data, a.len * sizeof(typename ListType::CElementType));
  memcpy(result.data + a.len, b.data, b.len * sizeof(typename ListType::CElementType));
  return result;
}

template <typename ListType>
uint8_t IsInList(typename ListType::CElementType a, ListType b) {
  if constexpr (std::is_same_v<float, typename ListType::CElementType>) {
    for (std::size_t i = 0; i < b.len; ++i) {
      if (std::abs(a - b.data[i]) < std::numeric_limits<float>::epsilon()) {
        return 1;
      }
    }
    return 0;
  } else if constexpr (std::is_same_v<double, typename ListType::CElementType>) {
    for (std::size_t i = 0; i < b.len; ++i) {
      if (std::abs(a - b.data[i]) < std::numeric_limits<double>::epsilon()) {
        return 1;
      }
    }
    return 0;
  } else {
    for (std::size_t i = 0; i < b.len; ++i) {
      if (a == b.data[i]) {
        return 1;
      }
    }
    return 0;
  }
}

inline uint8_t IsInStringList(StringStruct a, StringListStruct b) {
  std::string_view a_view(a.data, a.len);
  for (std::size_t i = 0; i < b.len; ++i) {
    std::string_view b_view(b.data[i].data, b.data[i].len);
    if (a_view == b_view) {
      return 1;
    }
  }
  return 0;
}

llvm::Value *CallBuiltinLenFunction(const FunctionSignature & /*sign*/,
                                    const std::vector<llvm::Type *> & /*arg_llvm_type_list*/,
                                    const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  llvm::Value *len_value = ctx.builder.CreateExtractValue(arg_llvm_value_list[0], {1}, "len_value");
  return len_value;
}

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

template <typename ListType>
inline ListType SortAsc(ListType a, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  memcpy(result.data, a.data, a.len * sizeof(typename ListType::CElementType));
  std::sort(result.data, result.data + result.len);
  return result;
}

template <typename ListType>
inline ListType SortDesc(ListType a, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ListType result;
  result.data = reinterpret_cast<typename ListType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ListType::CElementType)));
  result.len = a.len;
  memcpy(result.data, a.data, a.len * sizeof(typename ListType::CElementType));
  std::sort(result.data, result.data + result.len, std::greater<typename ListType::CElementType>());
  return result;
}

llvm::Value *CallBuiltinTruncateFunction(const FunctionSignature & /*sign*/,
                                         const std::vector<llvm::Type *> &arg_llvm_type_list,
                                         const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  llvm::Value *result_ptr = ctx.builder.CreateAlloca(arg_llvm_type_list[0], nullptr, "create result ptr");
  ctx.builder.CreateStore(arg_llvm_value_list[0], result_ptr);
  llvm::Value *len_ptr = ctx.builder.CreateGEP(arg_llvm_type_list[0], result_ptr,
                                               {llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx.context), 0),
                                                llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx.context), 1)},
                                               "load len_ptr");

  llvm::Value *new_len = arg_llvm_value_list[1];
  llvm::Value *old_len = ctx.builder.CreateLoad(llvm::Type::getInt32Ty(ctx.context), len_ptr, "old_len");
  llvm::Value *cmp = ctx.builder.CreateICmpULT(new_len, old_len, "cmp_len");
  llvm::Value *final_len = ctx.builder.CreateSelect(cmp, new_len, old_len, "final_len");
  ctx.builder.CreateStore(final_len, len_ptr);
  llvm::Value *result = ctx.builder.CreateLoad(arg_llvm_type_list[0], result_ptr, "result");
  return result;
}

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

template <typename ListType>
uint32_t MurmurHash3X8632(ListType a) {
  uint32_t result;
  MurmurHash3_x86_32(a.data, a.len * sizeof(typename ListType::CElementType), 0x9747b28c, &result);
  return result;
}

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

Status InitListConcatFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListConcat", {ValueType::kU8List, ValueType::kU8List, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(ListConcat<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListConcat", {ValueType::kU16List, ValueType::kU16List, ValueType::kPtr}, ValueType::kU16List),
      reinterpret_cast<void *>(ListConcat<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListConcat", {ValueType::kU32List, ValueType::kU32List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(ListConcat<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListConcat", {ValueType::kU64List, ValueType::kU64List, ValueType::kPtr}, ValueType::kU64List),
      reinterpret_cast<void *>(ListConcat<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListConcat", {ValueType::kI8List, ValueType::kI8List, ValueType::kPtr}, ValueType::kI8List),
      reinterpret_cast<void *>(ListConcat<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListConcat", {ValueType::kI16List, ValueType::kI16List, ValueType::kPtr}, ValueType::kI16List),
      reinterpret_cast<void *>(ListConcat<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListConcat", {ValueType::kI32List, ValueType::kI32List, ValueType::kPtr}, ValueType::kI32List),
      reinterpret_cast<void *>(ListConcat<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListConcat", {ValueType::kI64List, ValueType::kI64List, ValueType::kPtr}, ValueType::kI64List),
      reinterpret_cast<void *>(ListConcat<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListConcat", {ValueType::kF32List, ValueType::kF32List, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(ListConcat<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListConcat", {ValueType::kF64List, ValueType::kF64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(ListConcat<F64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("ListConcat", {ValueType::kStringList, ValueType::kStringList, ValueType::kPtr},
                        ValueType::kStringList),
      reinterpret_cast<void *>(ListConcat<StringListStruct>)));
  return Status::OK();
}

Status InitInFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("in", {ValueType::kU8, ValueType::kU8List}, ValueType::kU8),
                                 reinterpret_cast<void *>(IsInList<U8ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("in", {ValueType::kU16, ValueType::kU16List}, ValueType::kU8),
                                 reinterpret_cast<void *>(IsInList<U16ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("in", {ValueType::kU32, ValueType::kU32List}, ValueType::kU8),
                                 reinterpret_cast<void *>(IsInList<U32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("in", {ValueType::kU64, ValueType::kU64List}, ValueType::kU8),
                                 reinterpret_cast<void *>(IsInList<U64ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("in", {ValueType::kI8, ValueType::kI8List}, ValueType::kU8),
                                 reinterpret_cast<void *>(IsInList<I8ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("in", {ValueType::kI16, ValueType::kI16List}, ValueType::kU8),
                                 reinterpret_cast<void *>(IsInList<I16ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("in", {ValueType::kI32, ValueType::kI32List}, ValueType::kU8),
                                 reinterpret_cast<void *>(IsInList<I32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("in", {ValueType::kI64, ValueType::kI64List}, ValueType::kU8),
                                 reinterpret_cast<void *>(IsInList<I64ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("in", {ValueType::kF32, ValueType::kF32List}, ValueType::kU8),
                                 reinterpret_cast<void *>(IsInList<F32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("in", {ValueType::kF64, ValueType::kF64List}, ValueType::kU8),
                                 reinterpret_cast<void *>(IsInList<F64ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("in", {ValueType::kString, ValueType::kStringList}, ValueType::kU8),
                                 reinterpret_cast<void *>(IsInStringList)));
  return Status::OK();
}

Status InitLenFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(FunctionSignature("Len", {ValueType::kU8List}, ValueType::kU32),
                                                 CallBuiltinLenFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(FunctionSignature("Len", {ValueType::kU16List}, ValueType::kU32),
                                                 CallBuiltinLenFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(FunctionSignature("Len", {ValueType::kU32List}, ValueType::kU32),
                                                 CallBuiltinLenFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(FunctionSignature("Len", {ValueType::kU64List}, ValueType::kU32),
                                                 CallBuiltinLenFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(FunctionSignature("Len", {ValueType::kI8List}, ValueType::kU32),
                                                 CallBuiltinLenFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(FunctionSignature("Len", {ValueType::kI16List}, ValueType::kU32),
                                                 CallBuiltinLenFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(FunctionSignature("Len", {ValueType::kI32List}, ValueType::kU32),
                                                 CallBuiltinLenFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(FunctionSignature("Len", {ValueType::kI64List}, ValueType::kU32),
                                                 CallBuiltinLenFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(FunctionSignature("Len", {ValueType::kF32List}, ValueType::kU32),
                                                 CallBuiltinLenFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(FunctionSignature("Len", {ValueType::kF64List}, ValueType::kU32),
                                                 CallBuiltinLenFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(FunctionSignature("Len", {ValueType::kStringList}, ValueType::kU32),
                                                 CallBuiltinLenFunction));
  return Status::OK();
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

Status InitSortFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortAsc", {ValueType::kU8List, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(SortAsc<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortAsc", {ValueType::kU16List, ValueType::kPtr}, ValueType::kU16List),
      reinterpret_cast<void *>(SortAsc<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortAsc", {ValueType::kU32List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(SortAsc<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortAsc", {ValueType::kU64List, ValueType::kPtr}, ValueType::kU64List),
      reinterpret_cast<void *>(SortAsc<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortAsc", {ValueType::kI8List, ValueType::kPtr}, ValueType::kI8List),
      reinterpret_cast<void *>(SortAsc<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortAsc", {ValueType::kI16List, ValueType::kPtr}, ValueType::kI16List),
      reinterpret_cast<void *>(SortAsc<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortAsc", {ValueType::kI32List, ValueType::kPtr}, ValueType::kI32List),
      reinterpret_cast<void *>(SortAsc<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortAsc", {ValueType::kI64List, ValueType::kPtr}, ValueType::kI64List),
      reinterpret_cast<void *>(SortAsc<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortAsc", {ValueType::kF32List, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(SortAsc<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortAsc", {ValueType::kF64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(SortAsc<F64ListStruct>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortDesc", {ValueType::kU8List, ValueType::kPtr}, ValueType::kU8List),
      reinterpret_cast<void *>(SortDesc<U8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortDesc", {ValueType::kU16List, ValueType::kPtr}, ValueType::kU16List),
      reinterpret_cast<void *>(SortDesc<U16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortDesc", {ValueType::kU32List, ValueType::kPtr}, ValueType::kU32List),
      reinterpret_cast<void *>(SortDesc<U32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortDesc", {ValueType::kU64List, ValueType::kPtr}, ValueType::kU64List),
      reinterpret_cast<void *>(SortDesc<U64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortDesc", {ValueType::kI8List, ValueType::kPtr}, ValueType::kI8List),
      reinterpret_cast<void *>(SortDesc<I8ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortDesc", {ValueType::kI16List, ValueType::kPtr}, ValueType::kI16List),
      reinterpret_cast<void *>(SortDesc<I16ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortDesc", {ValueType::kI32List, ValueType::kPtr}, ValueType::kI32List),
      reinterpret_cast<void *>(SortDesc<I32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortDesc", {ValueType::kI64List, ValueType::kPtr}, ValueType::kI64List),
      reinterpret_cast<void *>(SortDesc<I64ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortDesc", {ValueType::kF32List, ValueType::kPtr}, ValueType::kF32List),
      reinterpret_cast<void *>(SortDesc<F32ListStruct>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("SortDesc", {ValueType::kF64List, ValueType::kPtr}, ValueType::kF64List),
      reinterpret_cast<void *>(SortDesc<F64ListStruct>)));
  return Status::OK();
}

Status InitTruncateFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("Truncate", {ValueType::kU8List, ValueType::kU32}, ValueType::kU8List),
      CallBuiltinTruncateFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("Truncate", {ValueType::kU16List, ValueType::kU32}, ValueType::kU16List),
      CallBuiltinTruncateFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("Truncate", {ValueType::kU32List, ValueType::kU32}, ValueType::kU32List),
      CallBuiltinTruncateFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("Truncate", {ValueType::kU64List, ValueType::kU32}, ValueType::kU64List),
      CallBuiltinTruncateFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("Truncate", {ValueType::kI8List, ValueType::kU32}, ValueType::kI8List),
      CallBuiltinTruncateFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("Truncate", {ValueType::kI16List, ValueType::kU32}, ValueType::kI16List),
      CallBuiltinTruncateFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("Truncate", {ValueType::kI32List, ValueType::kU32}, ValueType::kI32List),
      CallBuiltinTruncateFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("Truncate", {ValueType::kI64List, ValueType::kU32}, ValueType::kI64List),
      CallBuiltinTruncateFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("Truncate", {ValueType::kF32List, ValueType::kU32}, ValueType::kF32List),
      CallBuiltinTruncateFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("Truncate", {ValueType::kF64List, ValueType::kU32}, ValueType::kF64List),
      CallBuiltinTruncateFunction));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(
      FunctionSignature("Truncate", {ValueType::kStringList, ValueType::kU32}, ValueType::kStringList),
      CallBuiltinTruncateFunction));
  return Status::OK();
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

Status InitCountBitsFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(FunctionSignature("CountBits", {ValueType::kU8List}, ValueType::kU32),
                                              reinterpret_cast<void *>(CountBits)));
  return Status::OK();
}

Status InitHashFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("MurmurHash3X8632", {ValueType::kU8List}, ValueType::kU32),
                                 reinterpret_cast<void *>(MurmurHash3X8632<U8ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("MurmurHash3X8632", {ValueType::kU16List}, ValueType::kU32),
                                 reinterpret_cast<void *>(MurmurHash3X8632<U16ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("MurmurHash3X8632", {ValueType::kU32List}, ValueType::kU32),
                                 reinterpret_cast<void *>(MurmurHash3X8632<U32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("MurmurHash3X8632", {ValueType::kU64List}, ValueType::kU32),
                                 reinterpret_cast<void *>(MurmurHash3X8632<U64ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("MurmurHash3X8632", {ValueType::kI8List}, ValueType::kU32),
                                 reinterpret_cast<void *>(MurmurHash3X8632<I8ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("MurmurHash3X8632", {ValueType::kI16List}, ValueType::kU32),
                                 reinterpret_cast<void *>(MurmurHash3X8632<I16ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("MurmurHash3X8632", {ValueType::kI32List}, ValueType::kU32),
                                 reinterpret_cast<void *>(MurmurHash3X8632<I32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("MurmurHash3X8632", {ValueType::kI64List}, ValueType::kU32),
                                 reinterpret_cast<void *>(MurmurHash3X8632<I64ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("MurmurHash3X8632", {ValueType::kF32List}, ValueType::kU32),
                                 reinterpret_cast<void *>(MurmurHash3X8632<F32ListStruct>)));
  JF_RETURN_NOT_OK(
      reg->RegisterReadOnlyCFunc(FunctionSignature("MurmurHash3X8632", {ValueType::kF64List}, ValueType::kU32),
                                 reinterpret_cast<void *>(MurmurHash3X8632<F64ListStruct>)));
  return Status::OK();
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

Status InitOperationFunc(FunctionRegistry *reg) {
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
};

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
  JF_RETURN_NOT_OK(InitOperationFunc(reg));
  JF_RETURN_NOT_OK(InitFilterFunc(reg));
  JF_RETURN_NOT_OK(InitCountBitsFunc(reg));
  JF_RETURN_NOT_OK(InitHashFunc(reg));
  JF_RETURN_NOT_OK(InitIfFunc(reg));
  return Status::OK();
}

}  // namespace jitfusion
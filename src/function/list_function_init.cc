/*
 * @Author: victorika
 * @Date: 2025-01-23 12:39:23
 * @Last Modified by: victorika
 * @Last Modified time: 2026-02-09 15:04:26
 */
#include <algorithm>
#include <cmath>
#include <cstring>
#include <string_view>
#include <unordered_set>
#include "exec_engine.h"
#include "function_init.h"
#include "function_registry.h"
#include "llvm/IR/Value.h"
#include "murmurhash3.h"
#include "status.h"
#include "type.h"

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
uint32_t MurmurHash3X8632(ListType a) {
  uint32_t result;
  MurmurHash3_x86_32(a.data, a.len * sizeof(typename ListType::CElementType), 0x9747b28c, &result);
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

}  // namespace

Status InitListInternalFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(InitListArithmeticFunc(reg));
  JF_RETURN_NOT_OK(InitListComparisonFunc(reg));
  JF_RETURN_NOT_OK(InitListAggregationFunc(reg));

  JF_RETURN_NOT_OK(InitListConcatFunc(reg));
  JF_RETURN_NOT_OK(InitInFunc(reg));
  JF_RETURN_NOT_OK(InitLenFunc(reg));
  JF_RETURN_NOT_OK(InitSortFunc(reg));
  JF_RETURN_NOT_OK(InitTruncateFunc(reg));
  JF_RETURN_NOT_OK(InitHashFunc(reg));
  return Status::OK();
}

}  // namespace jitfusion
/*
 * @Author: victorika
 * @Date: 2025-01-23 12:39:23
 * @Last Modified by: victorika
 * @Last Modified time: 2025-04-02 17:04:12
 */
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include "codegen/codegen.h"
#include "exec_engine.h"
#include "function_init.h"
#include "function_registry.h"
#include "llvm/IR/Value.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {

llvm::Value *CallBuiltinListConcatFunction(const FunctionSignature &sign,
                                           const std::vector<llvm::Type *> & /*arg_llvm_type_list*/,
                                           const std::vector<llvm::Value *> &arg_llvm_value_list,
                                           IRCodeGenContext &ctx) {
  llvm::Value *a = arg_llvm_value_list[0];
  llvm::Value *b = arg_llvm_value_list[1];
  llvm::Value *a_data_ptr = ctx.builder.CreateExtractValue(a, {0});
  llvm::Value *a_len = ctx.builder.CreateExtractValue(a, {1});
  llvm::Value *b_data_ptr = ctx.builder.CreateExtractValue(b, {0});
  llvm::Value *b_len = ctx.builder.CreateExtractValue(b, {1});
  llvm::Value *result_len = ctx.builder.CreateAdd(a_len, b_len);
  auto element_size = TypeHelper::GetElementSize(sign.GetparamTypes().at(0));
  auto *llvm_element_size = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx.context), element_size, false);
  llvm::Value *result_data_ptr = CallAllocFunc(ctx, ctx.builder.CreateMul(result_len, llvm_element_size));
  llvm::Type *ret_llvm_type;
  CodeGen::ValueTypeToLLVMType(ctx, sign.GetRetType(), &ret_llvm_type);
  llvm::Value *result_ptr = ctx.builder.CreateAlloca(ret_llvm_type, nullptr, "create result ptr");
  llvm::Value *initial = llvm::UndefValue::get(ret_llvm_type);
  initial = ctx.builder.CreateInsertValue(initial, result_data_ptr, {0});
  initial = ctx.builder.CreateInsertValue(initial, result_len, {1});
  ctx.builder.CreateStore(initial, result_ptr);
  llvm::Value *result = ctx.builder.CreateLoad(ret_llvm_type, result_ptr, "result");

  llvm::Value *copy_a_size = ctx.builder.CreateMul(a_len, llvm_element_size);

  auto element_type = TypeHelper::GetElementType(sign.GetRetType());
  llvm::Type *ret_element_llvm_type;
  CodeGen::ValueTypeToLLVMType(ctx, element_type, &ret_element_llvm_type);
  auto *copy_b_start_ptr = ctx.builder.CreateGEP(ret_element_llvm_type, result_data_ptr, {a_len}, "copy_b_start_ptr");

  ctx.builder.CreateMemCpyInline(result_data_ptr, llvm::Align(1), a_data_ptr, llvm::Align(1), copy_a_size);
  ctx.builder.CreateMemCpyInline(copy_b_start_ptr, llvm::Align(1), b_data_ptr, llvm::Align(1),
                                 ctx.builder.CreateMul(b_len, llvm_element_size));
  return result;
}

template <typename ListType>
uint8_t IsInList(typename ListType::CElementType a, ListType b) {
  if constexpr (std::is_same_v<float, typename ListType::CElementType>) {
    for (size_t i = 0; i < b.len; ++i) {
      if (std::abs(a - b.data[i]) < std::numeric_limits<float>::epsilon()) {
        return 1;
      }
    }
    return 0;
  } else if constexpr (std::is_same_v<double, typename ListType::CElementType>) {
    for (size_t i = 0; i < b.len; ++i) {
      if (std::abs(a - b.data[i]) < std::numeric_limits<double>::epsilon()) {
        return 1;
      }
    }
    return 0;
  } else {
    for (size_t i = 0; i < b.len; ++i) {
      if (a == b.data[i]) {
        return 1;
      }
    }
    return 0;
  }
}

void IsInListAttributeSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
}

inline uint8_t IsInStringList(StringStruct a, StringListStruct b) {
  for (size_t i = 0; i < b.len; ++i) {
    if (a.len == b.data[i].len && strcmp(a.data, b.data[i].data) == 0) {
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

template <typename RetType, typename ListType>
inline RetType Sum(ListType a) {
  RetType sum = 0;
  for (size_t i = 0; i < a.len; ++i) {
    sum += a.data[i];
  }
  return sum;
}

void SumAttributeSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
}

template <typename ListType>
inline typename ListType::CElementType Max(ListType a) {
  return *std::max_element(a.data, a.data + a.len);
}

template <typename ListType>
inline typename ListType::CElementType Min(ListType a) {
  return *std::min_element(a.data, a.data + a.len);
}

void MinMaxAttributeSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
}

template <typename ListType>
inline uint32_t CountDistinct(ListType a) {
  std::unordered_set<typename ListType::CElementType> unique_set(a.data, a.data + a.len);
  return unique_set.size();
}

void CountDistinctAttributeSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
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

void SortAttributeSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
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
  ctx.builder.CreateStore(new_len, len_ptr);
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

void ListAddSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
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

void ListSubSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
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

void ListMulSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
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

void ListDivSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
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

void ListModSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
}

template <typename ListType, typename ResultType>
ResultType ListExp(ListType a, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ResultType result;
  result.data = reinterpret_cast<typename ResultType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ResultType::CElementType)));
  result.len = a.len;
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = std::exp(a.data[i]);
  }
  return result;
}

void ListExpSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
}

template <typename ListType, typename ResultType>
ResultType ListLog(ListType a, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  ResultType result;
  result.data = reinterpret_cast<typename ResultType::CElementType *>(
      exec_ctx->arena.Allocate((a.len) * sizeof(typename ResultType::CElementType)));
  result.len = a.len;
  for (uint32_t i = 0; i < a.len; i++) {
    result.data[i] = std::log(a.data[i]);
  }
  return result;
}

void ListLogSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
}

Status InitListConcatFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("ListConcat", {ValueType::kU8List, ValueType::kU8List}, ValueType::kU8List),
                        {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinListConcatFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kU16List, ValueType::kU16List}, ValueType::kU16List),
      {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinListConcatFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kU32List, ValueType::kU32List}, ValueType::kU32List),
      {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinListConcatFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kU64List, ValueType::kU64List}, ValueType::kU64List),
      {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinListConcatFunction}));
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("ListConcat", {ValueType::kI8List, ValueType::kI8List}, ValueType::kI8List),
                        {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinListConcatFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kI16List, ValueType::kI16List}, ValueType::kI16List),
      {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinListConcatFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kI32List, ValueType::kI32List}, ValueType::kI32List),
      {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinListConcatFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kI64List, ValueType::kI64List}, ValueType::kI64List),
      {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinListConcatFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kF32List, ValueType::kF32List}, ValueType::kF32List),
      {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinListConcatFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kF64List, ValueType::kF64List}, ValueType::kF64List),
      {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinListConcatFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListConcat", {ValueType::kStringList, ValueType::kStringList}, ValueType::kStringList),
      {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinListConcatFunction}));
  return Status::OK();
}

Status InitInFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("in", {ValueType::kU8, ValueType::kU8List}, ValueType::kU8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<U8ListStruct>), nullptr, IsInListAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("in", {ValueType::kU16, ValueType::kU16List}, ValueType::kU8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<U16ListStruct>), nullptr, IsInListAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("in", {ValueType::kU32, ValueType::kU32List}, ValueType::kU8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<U32ListStruct>), nullptr, IsInListAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("in", {ValueType::kU64, ValueType::kU64List}, ValueType::kU8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<U64ListStruct>), nullptr, IsInListAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("in", {ValueType::kI8, ValueType::kI8List}, ValueType::kU8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<I8ListStruct>), nullptr, IsInListAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("in", {ValueType::kI16, ValueType::kI16List}, ValueType::kU8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<I16ListStruct>), nullptr, IsInListAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("in", {ValueType::kI32, ValueType::kI32List}, ValueType::kU8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<I32ListStruct>), nullptr, IsInListAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("in", {ValueType::kI64, ValueType::kI64List}, ValueType::kU8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<I64ListStruct>), nullptr, IsInListAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("in", {ValueType::kF32, ValueType::kF32List}, ValueType::kU8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<F32ListStruct>), nullptr, IsInListAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("in", {ValueType::kF64, ValueType::kF64List}, ValueType::kU8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(IsInList<F64ListStruct>), nullptr, IsInListAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("in", {ValueType::kString, ValueType::kStringList}, ValueType::kU8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(IsInStringList), nullptr, IsInListAttributeSetter}));
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
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Sum", {ValueType::kU8List}, ValueType::kU64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<uint64_t, U8ListStruct>), nullptr, SumAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Sum", {ValueType::kI8List}, ValueType::kI64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<int64_t, I8ListStruct>), nullptr, SumAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Sum", {ValueType::kU16List}, ValueType::kU64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<uint64_t, U16ListStruct>), nullptr, SumAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Sum", {ValueType::kI16List}, ValueType::kI64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<int64_t, I16ListStruct>), nullptr, SumAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Sum", {ValueType::kU32List}, ValueType::kU64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<uint64_t, U32ListStruct>), nullptr, SumAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Sum", {ValueType::kI32List}, ValueType::kI64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<int64_t, I32ListStruct>), nullptr, SumAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Sum", {ValueType::kU64List}, ValueType::kU64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<uint64_t, U64ListStruct>), nullptr, SumAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Sum", {ValueType::kI64List}, ValueType::kI64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<int64_t, I64ListStruct>), nullptr, SumAttributeSetter}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Sum", {ValueType::kF32List}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<double, F32ListStruct>), nullptr, SumAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Sum", {ValueType::kF64List}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Sum<double, F64ListStruct>), nullptr, SumAttributeSetter}));
  return Status::OK();
}

Status InitMaxFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Max", {ValueType::kU8List}, ValueType::kU8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Max<U8ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Max", {ValueType::kI8List}, ValueType::kI8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Max<I8ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Max", {ValueType::kU16List}, ValueType::kU16),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Max<U16ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Max", {ValueType::kI16List}, ValueType::kI16),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Max<I16ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Max", {ValueType::kU32List}, ValueType::kU32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Max<U32ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Max", {ValueType::kI32List}, ValueType::kI32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Max<I32ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Max", {ValueType::kU64List}, ValueType::kU64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Max<U64ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Max", {ValueType::kI64List}, ValueType::kI64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Max<I64ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Max", {ValueType::kF32List}, ValueType::kF32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Max<F32ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Max", {ValueType::kF64List}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Max<F64ListStruct>), nullptr, MinMaxAttributeSetter}));
  return Status::OK();
}

Status InitMinFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Min", {ValueType::kU8List}, ValueType::kU8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Min<U8ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Min", {ValueType::kI8List}, ValueType::kI8),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Min<I8ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Min", {ValueType::kU16List}, ValueType::kU16),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Min<U16ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Min", {ValueType::kI16List}, ValueType::kI16),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Min<I16ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Min", {ValueType::kU32List}, ValueType::kU32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Min<U32ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Min", {ValueType::kI32List}, ValueType::kI32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Min<I32ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Min", {ValueType::kU64List}, ValueType::kU64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Min<U64ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Min", {ValueType::kI64List}, ValueType::kI64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Min<I64ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Min", {ValueType::kF32List}, ValueType::kF32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Min<F32ListStruct>), nullptr, MinMaxAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("Min", {ValueType::kF64List}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(Min<F64ListStruct>), nullptr, MinMaxAttributeSetter}));
  return Status::OK();
}

Status InitCountDistinctFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kU8List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<U8ListStruct>),
                                      nullptr, CountDistinctAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kI8List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<I8ListStruct>),
                                      nullptr, CountDistinctAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kU16List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<U16ListStruct>),
                                      nullptr, CountDistinctAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kI16List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<I16ListStruct>),
                                      nullptr, CountDistinctAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kU32List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<U32ListStruct>),
                                      nullptr, CountDistinctAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kI32List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<I32ListStruct>),
                                      nullptr, CountDistinctAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kU64List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<U64ListStruct>),
                                      nullptr, CountDistinctAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kI64List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<I64ListStruct>),
                                      nullptr, CountDistinctAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kF32List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<F32ListStruct>),
                                      nullptr, CountDistinctAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("CountDistinct", {ValueType::kF64List}, ValueType::kU32),
                                     {FunctionType::kCFunc, reinterpret_cast<void *>(CountDistinct<F64ListStruct>),
                                      nullptr, CountDistinctAttributeSetter}));
  return Status::OK();
}

Status InitSortFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortAsc", {ValueType::kU8List, ValueType::kPtr}, ValueType::kU8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<U8ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortAsc", {ValueType::kU16List, ValueType::kPtr}, ValueType::kU16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<U16ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortAsc", {ValueType::kU32List, ValueType::kPtr}, ValueType::kU32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<U32ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortAsc", {ValueType::kU64List, ValueType::kPtr}, ValueType::kU64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<U64ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortAsc", {ValueType::kI8List, ValueType::kPtr}, ValueType::kI8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<I8ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortAsc", {ValueType::kI16List, ValueType::kPtr}, ValueType::kI16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<I16ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortAsc", {ValueType::kI32List, ValueType::kPtr}, ValueType::kI32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<I32ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortAsc", {ValueType::kI64List, ValueType::kPtr}, ValueType::kI64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<I64ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortAsc", {ValueType::kF32List, ValueType::kPtr}, ValueType::kF32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<F32ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortAsc", {ValueType::kF64List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortAsc<F64ListStruct>), nullptr, SortAttributeSetter}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortDesc", {ValueType::kU8List, ValueType::kPtr}, ValueType::kU8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<U8ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortDesc", {ValueType::kU16List, ValueType::kPtr}, ValueType::kU16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<U16ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortDesc", {ValueType::kU32List, ValueType::kPtr}, ValueType::kU32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<U32ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortDesc", {ValueType::kU64List, ValueType::kPtr}, ValueType::kU64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<U64ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortDesc", {ValueType::kI8List, ValueType::kPtr}, ValueType::kI8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<I8ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortDesc", {ValueType::kI16List, ValueType::kPtr}, ValueType::kI16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<I16ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortDesc", {ValueType::kI32List, ValueType::kPtr}, ValueType::kI32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<I32ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortDesc", {ValueType::kI64List, ValueType::kPtr}, ValueType::kI64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<I64ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortDesc", {ValueType::kF32List, ValueType::kPtr}, ValueType::kF32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<F32ListStruct>), nullptr, SortAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("SortDesc", {ValueType::kF64List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(SortDesc<F64ListStruct>), nullptr, SortAttributeSetter}));
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

Status InitListAddFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListAdd", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListAdd<U8ListStruct>), nullptr, ListAddSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListAdd", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListAdd<U16ListStruct>), nullptr, ListAddSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListAdd", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListAdd<U32ListStruct>), nullptr, ListAddSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListAdd", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListAdd<U64ListStruct>), nullptr, ListAddSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListAdd", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListAdd<I8ListStruct>), nullptr, ListAddSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListAdd", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListAdd<I16ListStruct>), nullptr, ListAddSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListAdd", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListAdd<I32ListStruct>), nullptr, ListAddSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListAdd", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListAdd<I64ListStruct>), nullptr, ListAddSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListAdd", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr}, ValueType::kF32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListAdd<F32ListStruct>), nullptr, ListAddSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListAdd", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListAdd<F64ListStruct>), nullptr, ListAddSetter}));
  return Status::OK();
}

Status InitListSubFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListSub", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListSub<U8ListStruct>), nullptr, ListSubSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListSub", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListSub<U16ListStruct>), nullptr, ListSubSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListSub", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListSub<U32ListStruct>), nullptr, ListSubSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListSub", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListSub<U64ListStruct>), nullptr, ListSubSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListSub", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListSub<I8ListStruct>), nullptr, ListSubSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListSub", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListSub<I16ListStruct>), nullptr, ListSubSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListSub", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListSub<I32ListStruct>), nullptr, ListSubSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListSub", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListSub<I64ListStruct>), nullptr, ListSubSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListSub", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr}, ValueType::kF32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListSub<F32ListStruct>), nullptr, ListSubSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListSub", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListSub<F64ListStruct>), nullptr, ListSubSetter}));
  return Status::OK();
}

Status InitListMulFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMul", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMul<U8ListStruct>), nullptr, ListMulSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMul", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMul<U16ListStruct>), nullptr, ListMulSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMul", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMul<U32ListStruct>), nullptr, ListMulSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMul", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMul<U64ListStruct>), nullptr, ListMulSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMul", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMul<I8ListStruct>), nullptr, ListMulSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMul", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMul<I16ListStruct>), nullptr, ListMulSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMul", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMul<I32ListStruct>), nullptr, ListMulSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMul", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMul<I64ListStruct>), nullptr, ListMulSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMul", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr}, ValueType::kF32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMul<F32ListStruct>), nullptr, ListMulSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMul", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMul<F64ListStruct>), nullptr, ListMulSetter}));
  return Status::OK();
}

Status InitListDivFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListDiv", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListDiv<U8ListStruct>), nullptr, ListDivSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListDiv", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListDiv<U16ListStruct>), nullptr, ListDivSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListDiv", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListDiv<U32ListStruct>), nullptr, ListDivSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListDiv", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListDiv<U64ListStruct>), nullptr, ListDivSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListDiv", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListDiv<I8ListStruct>), nullptr, ListDivSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListDiv", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListDiv<I16ListStruct>), nullptr, ListDivSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListDiv", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListDiv<I32ListStruct>), nullptr, ListDivSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListDiv", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListDiv<I64ListStruct>), nullptr, ListDivSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListDiv", {ValueType::kF32List, ValueType::kF32, ValueType::kPtr}, ValueType::kF32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListDiv<F32ListStruct>), nullptr, ListDivSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListDiv", {ValueType::kF64List, ValueType::kF64, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListDiv<F64ListStruct>), nullptr, ListDivSetter}));
  return Status::OK();
}

Status InitListModFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMod", {ValueType::kU8List, ValueType::kU8, ValueType::kPtr}, ValueType::kU8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMod<U8ListStruct>), nullptr, ListModSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMod", {ValueType::kU16List, ValueType::kU16, ValueType::kPtr}, ValueType::kU16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMod<U16ListStruct>), nullptr, ListModSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMod", {ValueType::kU32List, ValueType::kU32, ValueType::kPtr}, ValueType::kU32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMod<U32ListStruct>), nullptr, ListModSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMod", {ValueType::kU64List, ValueType::kU64, ValueType::kPtr}, ValueType::kU64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMod<U64ListStruct>), nullptr, ListModSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMod", {ValueType::kI8List, ValueType::kI8, ValueType::kPtr}, ValueType::kI8List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMod<I8ListStruct>), nullptr, ListModSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMod", {ValueType::kI16List, ValueType::kI16, ValueType::kPtr}, ValueType::kI16List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMod<I16ListStruct>), nullptr, ListModSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMod", {ValueType::kI32List, ValueType::kI32, ValueType::kPtr}, ValueType::kI32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMod<I32ListStruct>), nullptr, ListModSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListMod", {ValueType::kI64List, ValueType::kI64, ValueType::kPtr}, ValueType::kI64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListMod<I64ListStruct>), nullptr, ListModSetter}));
  return Status::OK();
}

Status InitListExpFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListExp", {ValueType::kU8List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListExp<U8ListStruct, F64ListStruct>), nullptr, ListExpSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListExp", {ValueType::kU16List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListExp<U16ListStruct, F64ListStruct>), nullptr, ListExpSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListExp", {ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListExp<U32ListStruct, F64ListStruct>), nullptr, ListExpSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListExp", {ValueType::kU64List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListExp<U64ListStruct, F64ListStruct>), nullptr, ListExpSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListExp", {ValueType::kI8List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListExp<I8ListStruct, F64ListStruct>), nullptr, ListExpSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListExp", {ValueType::kI16List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListExp<I16ListStruct, F64ListStruct>), nullptr, ListExpSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListExp", {ValueType::kI32List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListExp<I32ListStruct, F64ListStruct>), nullptr, ListExpSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListExp", {ValueType::kI64List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListExp<I64ListStruct, F64ListStruct>), nullptr, ListExpSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListExp", {ValueType::kF32List, ValueType::kPtr}, ValueType::kF32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListExp<F32ListStruct, F32ListStruct>), nullptr, ListExpSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListExp", {ValueType::kF64List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListExp<F64ListStruct, F64ListStruct>), nullptr, ListExpSetter}));
  return Status::OK();
}

Status InitListLogFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListLog", {ValueType::kU8List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListLog<U8ListStruct, F64ListStruct>), nullptr, ListLogSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListLog", {ValueType::kU16List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListLog<U16ListStruct, F64ListStruct>), nullptr, ListLogSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListLog", {ValueType::kU32List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListLog<U32ListStruct, F64ListStruct>), nullptr, ListLogSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListLog", {ValueType::kU64List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListLog<U64ListStruct, F64ListStruct>), nullptr, ListLogSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListLog", {ValueType::kI8List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListLog<I8ListStruct, F64ListStruct>), nullptr, ListLogSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListLog", {ValueType::kI16List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListLog<I16ListStruct, F64ListStruct>), nullptr, ListLogSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListLog", {ValueType::kI32List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListLog<I32ListStruct, F64ListStruct>), nullptr, ListLogSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListLog", {ValueType::kI64List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListLog<I64ListStruct, F64ListStruct>), nullptr, ListLogSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListLog", {ValueType::kF32List, ValueType::kPtr}, ValueType::kF32List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListLog<F32ListStruct, F32ListStruct>), nullptr, ListLogSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ListLog", {ValueType::kF64List, ValueType::kPtr}, ValueType::kF64List),
      {FunctionType::kCFunc, reinterpret_cast<void *>(ListLog<F64ListStruct, F64ListStruct>), nullptr, ListLogSetter}));
  return Status::OK();
}

Status InitOperationFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(InitListAddFunc(reg));
  JF_RETURN_NOT_OK(InitListSubFunc(reg));
  JF_RETURN_NOT_OK(InitListMulFunc(reg));
  JF_RETURN_NOT_OK(InitListDivFunc(reg));
  JF_RETURN_NOT_OK(InitListModFunc(reg));
  JF_RETURN_NOT_OK(InitListExpFunc(reg));
  JF_RETURN_NOT_OK(InitListLogFunc(reg));
  return Status::OK();
};

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
  return Status::OK();
}

}  // namespace jitfusion
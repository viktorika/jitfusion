/*
 * @Author: victorika
 * @Date: 2025-01-23 12:36:51
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-29 22:44:52
 */
#include "codegen/codegen.h"
#include "exec_engine.h"
#include "function_init.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {

llvm::Value *CallBuiltinStringConcatFunction(const FunctionSignature & /*sign*/,
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
  auto element_size = StringStruct::kElementSize;
  auto *llvm_element_size = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx.context), element_size, false);
  llvm::Value *result_data_ptr = CallAllocFunc(
      ctx, ctx.builder.CreateMul(
               ctx.builder.CreateAdd(result_len, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx.context), 1, false)),
               llvm_element_size));
  llvm::Value *result_ptr = ctx.builder.CreateAlloca(ctx.complex_type.string_type, nullptr, "create result ptr");
  llvm::Value *initial = llvm::UndefValue::get(ctx.complex_type.string_type);
  initial = ctx.builder.CreateInsertValue(initial, result_data_ptr, {0});
  initial = ctx.builder.CreateInsertValue(initial, result_len, {1});
  ctx.builder.CreateStore(initial, result_ptr);
  llvm::Value *result = ctx.builder.CreateLoad(ctx.complex_type.string_type, result_ptr, "result");

  llvm::Value *copy_a_size = ctx.builder.CreateMul(a_len, llvm_element_size);

  auto *copy_b_start_ptr =
      ctx.builder.CreateGEP(llvm::Type::getInt8Ty(ctx.context), result_data_ptr, {a_len}, "copy_b_start_ptr");

  ctx.builder.CreateMemCpyInline(result_data_ptr, llvm::Align(1), a_data_ptr, llvm::Align(1), copy_a_size);
  ctx.builder.CreateMemCpyInline(copy_b_start_ptr, llvm::Align(1), b_data_ptr, llvm::Align(1),
                                 ctx.builder.CreateMul(b_len, llvm_element_size));
  // result_ptr[result_len] = '\0';
  ctx.builder.CreateStore(
      llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx.context), '\0', true),
      ctx.builder.CreateGEP(llvm::Type::getInt8Ty(ctx.context), result_data_ptr, {result_len}, "null terminator"));
  return result;
}

inline int32_t StringCmp(StringStruct a, StringStruct b) { return strcmp(a.data, b.data); }

void StringCmpAttributeSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
}

llvm::Value *CallBuiltinStringLenFunction(const FunctionSignature & /*sign*/,
                                          const std::vector<llvm::Type *> & /*arg_llvm_type_list*/,
                                          const std::vector<llvm::Value *> &arg_llvm_value_list,
                                          IRCodeGenContext &ctx) {
  llvm::Value *len_value = ctx.builder.CreateExtractValue(arg_llvm_value_list[0], {1}, "len_value");
  return len_value;
}

}  // namespace

Status InitStringInternalFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("StringConcat", {ValueType::kString, ValueType::kString}, ValueType::kString),
                        {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinStringConcatFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("StringCmp", {ValueType::kString, ValueType::kString}, ValueType::kI32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(StringCmp), nullptr, StringCmpAttributeSetter}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("StringLen", {ValueType::kString}, ValueType::kU32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinStringLenFunction}));
  return Status::OK();
}

}  // namespace jitfusion
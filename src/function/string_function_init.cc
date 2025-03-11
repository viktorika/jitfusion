/*
 * @Author: victorika
 * @Date: 2025-01-23 12:36:51
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-29 22:44:52
 */
#include "exec_engine.h"
#include "function_init.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {

inline int32_t StringCmp(LLVMComplexStruct a, LLVMComplexStruct b) {
  return strcmp(reinterpret_cast<char *>(a.data), reinterpret_cast<char *>(b.data));
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
      reg->RegisterFunc(FunctionSignature("StringCmp", {ValueType::kString, ValueType::kString}, ValueType::kI32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(StringCmp), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("StringLen", {ValueType::kString}, ValueType::kU32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinStringLenFunction}));
  return Status::OK();
}

}  // namespace jitfusion
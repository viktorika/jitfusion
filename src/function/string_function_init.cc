/*
 * @Author: victorika
 * @Date: 2025-01-23 12:36:51
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-29 22:44:52
 */
#include <cstring>
#include <string_view>
#include "codegen/codegen.h"
#include "exec_engine.h"
#include "function_init.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {

inline StringStruct StringConcat(StringStruct a, StringStruct b, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  StringStruct result;
  result.len = a.len + b.len;
  result.data = reinterpret_cast<char *>(exec_ctx->arena.Allocate((result.len + 1) * sizeof(char)));
  memcpy(result.data, a.data, a.len * sizeof(char));
  memcpy(result.data + a.len, b.data, b.len * sizeof(char));
  result.data[result.len] = '\0';
  return result;
}

inline int32_t StringCmp(StringStruct a, StringStruct b) {
  std::string_view va(a.data, a.len);
  std::string_view vb(b.data, b.len);
  return va.compare(vb);
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
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("StringConcat", {ValueType::kString, ValueType::kString, ValueType::kPtr}, ValueType::kString),
      reinterpret_cast<void *>(StringConcat)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("StringCmp", {ValueType::kString, ValueType::kString}, ValueType::kI32),
      reinterpret_cast<void *>(StringCmp)));
  JF_RETURN_NOT_OK(reg->RegisterLLVMIntrinicFunc(FunctionSignature("StringLen", {ValueType::kString}, ValueType::kU32),
                                                 CallBuiltinStringLenFunction));
  return Status::OK();
}

}  // namespace jitfusion
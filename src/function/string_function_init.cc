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

inline LLVMComplexStruct StringConcat(LLVMComplexStruct a, LLVMComplexStruct b, int64_t exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);

  LLVMComplexStruct result;
  result.data = reinterpret_cast<int64_t>(exec_ctx->arena.Allocate((a.len + b.len + 1) * sizeof(char)));
  result.len = a.len + b.len;
  memcpy(reinterpret_cast<char *>(result.data), reinterpret_cast<char *>(a.data), a.len * sizeof(char));
  memcpy(reinterpret_cast<char *>(result.data) + a.len, reinterpret_cast<char *>(b.data), b.len * sizeof(char));
  reinterpret_cast<char *>(result.data)[result.len] = '\0';
  return result;
}

inline int32_t StringCmp(LLVMComplexStruct a, LLVMComplexStruct b) {
  return strcmp(reinterpret_cast<char *>(a.data), reinterpret_cast<char *>(b.data));
}

inline uint32_t StringLen(LLVMComplexStruct a) { return a.len; }

}  // namespace

Status InitStringInternalFunc(FunctionRegistry *reg) {
  RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("StringConcat", {ValueType::kString, ValueType::kString, ValueType::kI64}, ValueType::kString),
      {FunctionType::kCFunc, reinterpret_cast<void *>(StringConcat), nullptr}));
  RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("StringCmp", {ValueType::kString, ValueType::kString}, ValueType::kI32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(StringCmp), nullptr}));
  RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("StringLen", {ValueType::kString}, ValueType::kU32),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(StringLen), nullptr}));
  return Status::OK();
}

}  // namespace jitfusion
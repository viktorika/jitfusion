/*
 * @Author: victorika
 * @Date: 2025-01-23 12:36:51
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-03 16:36:37
 */
#include <charconv>
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
  result.data = reinterpret_cast<char *>(exec_ctx->arena.Allocate(result.len * sizeof(char)));
  memcpy(result.data, a.data, a.len * sizeof(char));
  memcpy(result.data + a.len, b.data, b.len * sizeof(char));
  return result;
}

inline int32_t StringCmp(StringStruct a, StringStruct b) {
  std::string_view va(a.data, a.len);
  std::string_view vb(b.data, b.len);
  return va.compare(vb);
}

template <typename T>
inline StringStruct CastNumericToString(T value, void *exec_context) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  char buf[64];
  auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), value);
  int written = static_cast<int>(ptr - buf);
  StringStruct result;
  result.data = reinterpret_cast<char *>(exec_ctx->arena.Allocate(written));
  memcpy(result.data, buf, written);
  result.len = written;
  return result;
}

template <typename T>
inline T CastStringToNumeric(StringStruct a) {
  static_assert(!std::is_floating_point_v<T>, "C++17 std::from_chars does not support floating point types");
  const char *begin = a.data;
  const char *end = begin + a.len;
  T val{};
  std::from_chars(begin, end, val);
  return val;
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

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastString", {ValueType::kU8, ValueType::kPtr}, ValueType::kString),
      reinterpret_cast<void *>(CastNumericToString<uint8_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastString", {ValueType::kI8, ValueType::kPtr}, ValueType::kString),
      reinterpret_cast<void *>(CastNumericToString<int8_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastString", {ValueType::kU16, ValueType::kPtr}, ValueType::kString),
      reinterpret_cast<void *>(CastNumericToString<uint16_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastString", {ValueType::kI16, ValueType::kPtr}, ValueType::kString),
      reinterpret_cast<void *>(CastNumericToString<int16_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastString", {ValueType::kU32, ValueType::kPtr}, ValueType::kString),
      reinterpret_cast<void *>(CastNumericToString<uint32_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastString", {ValueType::kI32, ValueType::kPtr}, ValueType::kString),
      reinterpret_cast<void *>(CastNumericToString<int32_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastString", {ValueType::kU64, ValueType::kPtr}, ValueType::kString),
      reinterpret_cast<void *>(CastNumericToString<uint64_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastString", {ValueType::kI64, ValueType::kPtr}, ValueType::kString),
      reinterpret_cast<void *>(CastNumericToString<int64_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastString", {ValueType::kF32, ValueType::kPtr}, ValueType::kString),
      reinterpret_cast<void *>(CastNumericToString<float>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastString", {ValueType::kF64, ValueType::kPtr}, ValueType::kString),
      reinterpret_cast<void *>(CastNumericToString<double>)));

  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastU8", {ValueType::kString}, ValueType::kU8),
      reinterpret_cast<void *>(CastStringToNumeric<uint8_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastI8", {ValueType::kString}, ValueType::kI8),
      reinterpret_cast<void *>(CastStringToNumeric<int8_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastU16", {ValueType::kString}, ValueType::kU16),
      reinterpret_cast<void *>(CastStringToNumeric<uint16_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastI16", {ValueType::kString}, ValueType::kI16),
      reinterpret_cast<void *>(CastStringToNumeric<int16_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastU32", {ValueType::kString}, ValueType::kU32),
      reinterpret_cast<void *>(CastStringToNumeric<uint32_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastI32", {ValueType::kString}, ValueType::kI32),
      reinterpret_cast<void *>(CastStringToNumeric<int32_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastU64", {ValueType::kString}, ValueType::kU64),
      reinterpret_cast<void *>(CastStringToNumeric<uint64_t>)));
  JF_RETURN_NOT_OK(reg->RegisterReadOnlyCFunc(
      FunctionSignature("CastI64", {ValueType::kString}, ValueType::kI64),
      reinterpret_cast<void *>(CastStringToNumeric<int64_t>)));

  return Status::OK();
}

}  // namespace jitfusion
/*
 * @Author: victorika
 * @Date: 2025-01-23 10:31:11
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-23 17:09:36
 */
#include <cmath>
#include "codegen/codegen.h"
#include "function_init.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {
// Numeric
template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
inline double exp(T x) {
  return std::exp(x);
}
inline float exp(float x) { return std::exp(x); }
inline double exp(double x) { return std::exp(x); }

template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
inline double log(T x) {
  return std::log(x);
}
inline float log(float x) { return std::log(x); }
inline double log(double x) { return std::log(x); }

template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
inline double log2(T x) {
  return std::log2(x);
}
inline float log2(float x) { return std::log2(x); }
inline double log2(double x) { return std::log2(x); }

template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
inline double log10(T x) {
  return std::log10(x);
}
inline float log10(float x) { return std::log10(x); }
inline double log10(double x) { return std::log10(x); }

inline float frac(float x) { return x - std::trunc(x); }
inline double frac(double x) { return x - std::trunc(x); }

inline float trunc(float x) { return std::trunc(x); }
inline double trunc(double x) { return std::trunc(x); }

inline double pow(float x, int32_t y) { return std::pow(x, y); }
inline float pow(float x, float y) { return std::pow(x, y); }
inline double pow(double x, int32_t y) { return std::pow(x, y); }
inline double pow(double x, double y) { return std::pow(x, y); }
inline double pow(int32_t x, int32_t y) { return std::pow(x, y); }
inline double pow(int64_t x, int32_t y) { return std::pow(x, y); }

inline float sin(float x) { return std::sin(x); }
inline double sin(double x) { return std::sin(x); }

inline float cos(float x) { return std::cos(x); }
inline double cos(double x) { return std::cos(x); }

inline float tan(float x) { return std::tan(x); }
inline double tan(double x) { return std::tan(x); }

inline float sqrt(float x) { return std::sqrt(x); }
inline double sqrt(double x) { return std::sqrt(x); }

inline float abs(float x) { return std::abs(x); }
inline double abs(double x) { return std::abs(x); }

#if defined(__clang__)
#  define NO_OPTIMIZE __attribute__((optnone))
#elif defined(__GNUC__)
#  define NO_OPTIMIZE __attribute__((optimize("O0")))
#elif defined(_MSC_VER)
#  define NO_OPTIMIZE
#  pragma optimize("", off)
#else
#  define NO_OPTIMIZE
#endif
NO_OPTIMIZE inline int32_t abs(int8_t x) { return std::abs(x); }

inline int32_t abs(int16_t x) { return std::abs(x); }
inline int32_t abs(int32_t x) { return std::abs(x); }
inline int64_t abs(int64_t x) { return std::abs(x); }

inline float ceil(float x) { return std::ceil(x); }
inline double ceil(double x) { return std::ceil(x); }

inline float floor(float x) { return std::floor(x); }
inline double floor(double x) { return std::floor(x); }

inline float round(float x) { return std::round(x); }
inline double round(double x) { return std::round(x); }

llvm::Value *CallBuiltinMinFunction(const FunctionSignature &sign,
                                    const std::vector<llvm::Type *> & /*arg_llvm_type_list*/,
                                    const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  llvm::Value *condition = TypeHelper::IsIntegerType(sign.GetRetType())
                               ? TypeHelper::IsSignedType(sign.GetRetType())
                                     ? ctx.builder.CreateICmpSGT(arg_llvm_value_list.at(0), arg_llvm_value_list.at(1))
                                     : ctx.builder.CreateICmpUGT(arg_llvm_value_list.at(0), arg_llvm_value_list.at(1))
                               : ctx.builder.CreateFCmpUGT(arg_llvm_value_list.at(0), arg_llvm_value_list.at(1));
  return ctx.builder.CreateSelect(condition, arg_llvm_value_list.at(1), arg_llvm_value_list.at(0), "min");
};

llvm::Value *CallBuiltinMaxFunction(const FunctionSignature &sign,
                                    const std::vector<llvm::Type *> & /*arg_llvm_type_list*/,
                                    const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  llvm::Value *condition = TypeHelper::IsIntegerType(sign.GetRetType())
                               ? TypeHelper::IsSignedType(sign.GetRetType())
                                     ? ctx.builder.CreateICmpSGT(arg_llvm_value_list.at(0), arg_llvm_value_list.at(1))
                                     : ctx.builder.CreateICmpUGT(arg_llvm_value_list.at(0), arg_llvm_value_list.at(1))
                               : ctx.builder.CreateFCmpUGT(arg_llvm_value_list.at(0), arg_llvm_value_list.at(1));
  return ctx.builder.CreateSelect(condition, arg_llvm_value_list.at(0), arg_llvm_value_list.at(1), "max");
};

llvm::Value *CallBuiltinCastFunction(const FunctionSignature &sign,
                                     const std::vector<llvm::Type *> & /*arg_llvm_type_list*/,
                                     const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *result = arg_llvm_value_list.back();
  CodeGen::NumericTypeConvert(ctx, sign.GetparamTypes().back(), sign.GetRetType(), &result);
  return result;
};

Status InitExpFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("exp", {ValueType::kI32}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(exp<int32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("exp", {ValueType::kI64}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(exp<int64_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("exp", {ValueType::kU32}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(exp<uint32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("exp", {ValueType::kU64}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(exp<uint64_t>), nullptr}));

  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("exp", {ValueType::kF32}, ValueType::kF32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float)>(exp)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("exp", {ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double)>(exp)), nullptr}));
  return Status::OK();
}

Status InitLogFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log", {ValueType::kI32}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(log<int32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log", {ValueType::kI64}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(log<int64_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log", {ValueType::kU32}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(log<uint32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log", {ValueType::kU64}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(log<uint64_t>), nullptr}));

  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("log", {ValueType::kF32}, ValueType::kF32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float)>(log)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("log", {ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double)>(log)), nullptr}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log2", {ValueType::kI32}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(log2<int32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log2", {ValueType::kI64}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(log2<int64_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log2", {ValueType::kU32}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(log2<uint32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log2", {ValueType::kU64}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(log2<uint64_t>), nullptr}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("log2", {ValueType::kF32}, ValueType::kF32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float)>(log2)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("log2", {ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double)>(log2)), nullptr}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log10", {ValueType::kI32}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(log10<int32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log10", {ValueType::kI64}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(log10<int64_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log10", {ValueType::kU32}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(log10<uint32_t>), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log10", {ValueType::kU64}, ValueType::kF64),
                                  {FunctionType::kCFunc, reinterpret_cast<void *>(log10<uint64_t>), nullptr}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("log10", {ValueType::kF32}, ValueType::kF32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float)>(log10)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("log10", {ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double)>(log10)), nullptr}));

  return Status::OK();
}

Status InitFracFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("frac", {ValueType::kF32}, ValueType::kF32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float)>(frac)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("frac", {ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double)>(frac)), nullptr}));
  return Status::OK();
}

Status InitTruncFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("trunc", {ValueType::kF32}, ValueType::kF32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float)>(trunc)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("trunc", {ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double)>(trunc)), nullptr}));
  return Status::OK();
}

Status InitPowFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("pow", {ValueType::kF32, ValueType::kI32}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(float, int32_t)>(pow)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("pow", {ValueType::kF64, ValueType::kI32}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double, int32_t)>(pow)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("pow", {ValueType::kI32, ValueType::kI32}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(int32_t, int32_t)>(pow)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("pow", {ValueType::kI64, ValueType::kI32}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(int64_t, int32_t)>(pow)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("pow", {ValueType::kF32, ValueType::kF32}, ValueType::kF32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float, float)>(pow)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("pow", {ValueType::kF64, ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double, double)>(pow)), nullptr}));
  return Status::OK();
}

Status InitTrigonometricFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("sin", {ValueType::kF32}, ValueType::kF32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float)>(sin)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("sin", {ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double)>(sin)), nullptr}));

  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("cos", {ValueType::kF32}, ValueType::kF32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float)>(cos)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("cos", {ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double)>(cos)), nullptr}));

  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("tan", {ValueType::kF32}, ValueType::kF32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float)>(tan)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("tan", {ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double)>(tan)), nullptr}));
  return Status::OK();
}

Status InitSqrtFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("sqrt", {ValueType::kF32}, ValueType::kF32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float)>(sqrt)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("sqrt", {ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double)>(sqrt)), nullptr}));
  return Status::OK();
}

Status InitAbsFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("abs", {ValueType::kF32}, ValueType::kF32),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float)>(abs)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("abs", {ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double)>(abs)), nullptr}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("abs", {ValueType::kI8}, ValueType::kI32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<int32_t (*)(int8_t)>(abs)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("abs", {ValueType::kI16}, ValueType::kI32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<int32_t (*)(int16_t)>(abs)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("abs", {ValueType::kI32}, ValueType::kI32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<int32_t (*)(int32_t)>(abs)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("abs", {ValueType::kI64}, ValueType::kI64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<int64_t (*)(int64_t)>(abs)), nullptr}));
  return Status::OK();
}

Status InitCeilFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ceil", {ValueType::kF32}, ValueType::kF32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float)>(ceil)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("ceil", {ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double)>(ceil)), nullptr}));
  return Status::OK();
}

Status InitFloorFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("floor", {ValueType::kF32}, ValueType::kF32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float)>(floor)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("floor", {ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double)>(floor)), nullptr}));
  return Status::OK();
}

Status InitRoundFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("round", {ValueType::kF32}, ValueType::kF32),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<float (*)(float)>(round)), nullptr}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(
      FunctionSignature("round", {ValueType::kF64}, ValueType::kF64),
      {FunctionType::kCFunc, reinterpret_cast<void *>(static_cast<double (*)(double)>(round)), nullptr}));
  return Status::OK();
}

Status InitMinFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("min", {ValueType::kU8, ValueType::kU8}, ValueType::kU8),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMinFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("min", {ValueType::kU16, ValueType::kU16}, ValueType::kU16),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMinFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("min", {ValueType::kU32, ValueType::kU32}, ValueType::kU32),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMinFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("min", {ValueType::kU64, ValueType::kU64}, ValueType::kU64),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMinFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("min", {ValueType::kI8, ValueType::kI8}, ValueType::kI8),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMinFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("min", {ValueType::kI16, ValueType::kI16}, ValueType::kI16),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMinFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("min", {ValueType::kI32, ValueType::kI32}, ValueType::kI32),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMinFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("min", {ValueType::kI64, ValueType::kI64}, ValueType::kI64),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMinFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("min", {ValueType::kF32, ValueType::kF32}, ValueType::kF32),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMinFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("min", {ValueType::kF64, ValueType::kF64}, ValueType::kF64),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMinFunction}));
  return Status::OK();
}

Status InitMaxFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("max", {ValueType::kU8, ValueType::kU8}, ValueType::kU8),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMaxFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("max", {ValueType::kU16, ValueType::kU16}, ValueType::kU16),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMaxFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("max", {ValueType::kU32, ValueType::kU32}, ValueType::kU32),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMaxFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("max", {ValueType::kU64, ValueType::kU64}, ValueType::kU64),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMaxFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("max", {ValueType::kI8, ValueType::kI8}, ValueType::kI8),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMaxFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("max", {ValueType::kI16, ValueType::kI16}, ValueType::kI16),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMaxFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("max", {ValueType::kI32, ValueType::kI32}, ValueType::kI32),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMaxFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("max", {ValueType::kI64, ValueType::kI64}, ValueType::kI64),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMaxFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("max", {ValueType::kF32, ValueType::kF32}, ValueType::kF32),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMaxFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("max", {ValueType::kF64, ValueType::kF64}, ValueType::kF64),
                                  {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinMaxFunction}));
  return Status::OK();
}

Status InitCastFunc(FunctionRegistry *reg) {
  static const std::vector<ValueType> kTypeVec{ValueType::kU8,  ValueType::kI8,  ValueType::kU16, ValueType::kI16,
                                               ValueType::kU32, ValueType::kI32, ValueType::kU64, ValueType::kI64,
                                               ValueType::kF32, ValueType::kF64};

  static const std::vector<std::pair<std::string, ValueType>> kNameWithType{
      {"CastU8", ValueType::kU8},   {"CastI8", ValueType::kI8},   {"CastU16", ValueType::kU16},
      {"CastI16", ValueType::kI16}, {"CastU32", ValueType::kU32}, {"CastI32", ValueType::kI32},
      {"CastU64", ValueType::kU64}, {"CastI64", ValueType::kI64}, {"CastF32", ValueType::kF32},
      {"CastF64", ValueType::kF64}};

  for (const auto &[func_name, ret_type] : kNameWithType) {
    for (auto args_type : kTypeVec) {
      JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature(func_name, {args_type}, ret_type),
                                      {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinCastFunction}));
    }
  }

  return Status::OK();
}

}  // namespace

Status InitMathInternalFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(InitExpFunc(reg));
  JF_RETURN_NOT_OK(InitLogFunc(reg));
  JF_RETURN_NOT_OK(InitFracFunc(reg));
  JF_RETURN_NOT_OK(InitTruncFunc(reg));
  JF_RETURN_NOT_OK(InitPowFunc(reg));
  JF_RETURN_NOT_OK(InitTrigonometricFunc(reg));
  JF_RETURN_NOT_OK(InitSqrtFunc(reg));
  JF_RETURN_NOT_OK(InitAbsFunc(reg));
  JF_RETURN_NOT_OK(InitCeilFunc(reg));
  JF_RETURN_NOT_OK(InitFloorFunc(reg));
  JF_RETURN_NOT_OK(InitRoundFunc(reg));
  JF_RETURN_NOT_OK(InitMinFunc(reg));
  JF_RETURN_NOT_OK(InitMaxFunc(reg));
  JF_RETURN_NOT_OK(InitCastFunc(reg));
  return Status::OK();
}

}  // namespace jitfusion
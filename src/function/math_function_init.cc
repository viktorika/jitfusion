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
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Type.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {

llvm::Value *CallBuiltinExpFunction(const FunctionSignature &sign,
                                    const std::vector<llvm::Type *> & /*arg_llvm_type_list*/,
                                    const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  CodeGen::NumericTypeConvert(ctx, sign.GetparamTypes().at(0), sign.GetRetType(), &value);
  llvm::Type *new_args_llvm_type;
  CodeGen::ValueTypeToLLVMType(ctx, sign.GetRetType(), &new_args_llvm_type);
  llvm::Function *func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::exp, new_args_llvm_type);
  return ctx.builder.CreateCall(func, value, "exp");
}

llvm::Value *CallBuiltinLogFunction(const FunctionSignature &sign,
                                    const std::vector<llvm::Type *> & /*arg_llvm_type_list*/,
                                    const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  CodeGen::NumericTypeConvert(ctx, sign.GetparamTypes().at(0), sign.GetRetType(), &value);
  llvm::Type *new_args_llvm_type;
  CodeGen::ValueTypeToLLVMType(ctx, sign.GetRetType(), &new_args_llvm_type);
  llvm::Function *func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::log, new_args_llvm_type);
  return ctx.builder.CreateCall(func, value, "log");
}

llvm::Value *CallBuiltinLog2Function(const FunctionSignature &sign,
                                     const std::vector<llvm::Type *> & /*arg_llvm_type_list*/,
                                     const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  CodeGen::NumericTypeConvert(ctx, sign.GetparamTypes().at(0), sign.GetRetType(), &value);
  llvm::Type *new_args_llvm_type;
  CodeGen::ValueTypeToLLVMType(ctx, sign.GetRetType(), &new_args_llvm_type);
  llvm::Function *func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::log2, new_args_llvm_type);
  return ctx.builder.CreateCall(func, value, "log2");
}

llvm::Value *CallBuiltinLog10Function(const FunctionSignature &sign,
                                      const std::vector<llvm::Type *> & /*arg_llvm_type_list*/,
                                      const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  CodeGen::NumericTypeConvert(ctx, sign.GetparamTypes().at(0), sign.GetRetType(), &value);
  llvm::Type *new_args_llvm_type;
  CodeGen::ValueTypeToLLVMType(ctx, sign.GetRetType(), &new_args_llvm_type);
  llvm::Function *func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::log10, new_args_llvm_type);
  return ctx.builder.CreateCall(func, value, "log10");
}

llvm::Value *CallBuiltinFracFunction(const FunctionSignature & /*sign*/,
                                     const std::vector<llvm::Type *> &arg_llvm_type_list,
                                     const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  llvm::Function *exp_func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::trunc, arg_llvm_type_list);
  llvm::Value *trunc = ctx.builder.CreateCall(exp_func, value, "trunc");
  return ctx.builder.CreateFSub(value, trunc, "frac");
}

llvm::Value *CallBuiltinTruncFunction(const FunctionSignature & /*sign*/,
                                      const std::vector<llvm::Type *> &arg_llvm_type_list,
                                      const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  llvm::Function *func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::trunc, arg_llvm_type_list);
  return ctx.builder.CreateCall(func, value, "trunc");
}

llvm::Value *CallBuiltinPowIFunction(const FunctionSignature &sign,
                                     const std::vector<llvm::Type *> & /*arg_llvm_type_list*/,
                                     const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  auto *power = arg_llvm_value_list.at(1);
  if (value->getType()->isIntegerTy()) {
    CodeGen::NumericTypeConvert(ctx, sign.GetparamTypes().at(0), ValueType::kF64, &value);
  }
  if (value->getType()->isDoubleTy()) {
    CodeGen::NumericTypeConvert(ctx, ValueType::kI32, ValueType::kI16, &power);
  }
  llvm::Function *func =
      llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::powi, {value->getType(), power->getType()});
  return ctx.builder.CreateCall(func, {value, power}, "powi");
}

llvm::Value *CallBuiltinPowFunction(const FunctionSignature & /*sign*/,
                                    const std::vector<llvm::Type *> & /*arg_llvm_type_list*/,
                                    const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  auto *power = arg_llvm_value_list.at(1);
  llvm::Function *func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::pow, {value->getType()});
  return ctx.builder.CreateCall(func, {value, power}, "pow");
}

llvm::Value *CallBuiltinSinFunction(const FunctionSignature & /*sign*/,
                                    const std::vector<llvm::Type *> &arg_llvm_type_list,
                                    const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  llvm::Function *func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::sin, arg_llvm_type_list);
  return ctx.builder.CreateCall(func, value, "sin");
}

llvm::Value *CallBuiltinCosFunction(const FunctionSignature & /*sign*/,
                                    const std::vector<llvm::Type *> &arg_llvm_type_list,
                                    const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  llvm::Function *func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::cos, arg_llvm_type_list);
  return ctx.builder.CreateCall(func, value, "cos");
}

llvm::Value *CallBuiltinTanFunction(const FunctionSignature & /*sign*/,
                                    const std::vector<llvm::Type *> &arg_llvm_type_list,
                                    const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  llvm::Function *func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::tan, arg_llvm_type_list);
  return ctx.builder.CreateCall(func, value, "tan");
}

llvm::Value *CallBuiltinSqrtFunction(const FunctionSignature & /*sign*/,
                                     const std::vector<llvm::Type *> &arg_llvm_type_list,
                                     const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  llvm::Function *func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::sqrt, arg_llvm_type_list);
  return ctx.builder.CreateCall(func, value, "sqrt");
}

llvm::Value *CallBuiltinAbsFunction(const FunctionSignature &sign, const std::vector<llvm::Type *> &arg_llvm_type_list,
                                    const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  if (TypeHelper::IsIntegerType(sign.GetRetType())) {
    CodeGen::NumericTypeConvert(ctx, sign.GetparamTypes().at(0), sign.GetRetType(), &value);
    llvm::Type *new_args_llvm_type;
    CodeGen::ValueTypeToLLVMType(ctx, sign.GetRetType(), &new_args_llvm_type);
    std::vector<llvm::Type *> abs_func_args_list = {new_args_llvm_type};
    llvm::Function *abs_func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::abs, abs_func_args_list);
    llvm::Value *nsw = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx.context), 1);
    llvm::Value *result = ctx.builder.CreateCall(abs_func, {value, nsw}, "abs");
    return result;
  }
  llvm::Function *fabs_func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::fabs,
                                                              llvm::ArrayRef<llvm::Type *>(arg_llvm_type_list));
  return ctx.builder.CreateCall(fabs_func, arg_llvm_value_list, "fabs");
}

llvm::Value *CallBuiltinCeilFunction(const FunctionSignature & /*sign*/,
                                     const std::vector<llvm::Type *> &arg_llvm_type_list,
                                     const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  llvm::Function *func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::ceil, arg_llvm_type_list);
  return ctx.builder.CreateCall(func, value, "ceil");
}

llvm::Value *CallBuiltinFloorFunction(const FunctionSignature & /*sign*/,
                                      const std::vector<llvm::Type *> &arg_llvm_type_list,
                                      const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  llvm::Function *func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::floor, arg_llvm_type_list);
  return ctx.builder.CreateCall(func, value, "floor");
}

llvm::Value *CallBuiltinRoundFunction(const FunctionSignature & /*sign*/,
                                      const std::vector<llvm::Type *> &arg_llvm_type_list,
                                      const std::vector<llvm::Value *> &arg_llvm_value_list, IRCodeGenContext &ctx) {
  auto *value = arg_llvm_value_list.at(0);
  llvm::Function *func = llvm::Intrinsic::getDeclaration(&ctx.module, llvm::Intrinsic::round, arg_llvm_type_list);
  return ctx.builder.CreateCall(func, value, "round");
}

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
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinExpFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("exp", {ValueType::kI64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinExpFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("exp", {ValueType::kU32}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinExpFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("exp", {ValueType::kU64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinExpFunction}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("exp", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinExpFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("exp", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinExpFunction}));
  return Status::OK();
}

Status InitLogFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log", {ValueType::kI32}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLogFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log", {ValueType::kI64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLogFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log", {ValueType::kU32}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLogFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log", {ValueType::kU64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLogFunction}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLogFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLogFunction}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log2", {ValueType::kI32}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLog2Function}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log2", {ValueType::kI64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLog2Function}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log2", {ValueType::kU32}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLog2Function}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log2", {ValueType::kU64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLog2Function}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log2", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLog2Function}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log2", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLog2Function}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log10", {ValueType::kI32}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLog10Function}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log10", {ValueType::kI64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLog10Function}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log10", {ValueType::kU32}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLog10Function}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log10", {ValueType::kU64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLog10Function}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log10", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLog10Function}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("log10", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinLog10Function}));

  return Status::OK();
}

Status InitFracFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("frac", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinFracFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("frac", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinFracFunction}));
  return Status::OK();
}

Status InitTruncFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("trunc", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTruncFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("trunc", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTruncFunction}));
  return Status::OK();
}

Status InitPowFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("pow", {ValueType::kF32, ValueType::kI32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinPowIFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("pow", {ValueType::kF64, ValueType::kI32}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinPowIFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("pow", {ValueType::kI32, ValueType::kI32}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinPowIFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("pow", {ValueType::kI64, ValueType::kI32}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinPowIFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("pow", {ValueType::kF32, ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinPowFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("pow", {ValueType::kF64, ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinPowFunction}));
  return Status::OK();
}

Status InitTrigonometricFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("sin", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinSinFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("sin", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinSinFunction}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("cos", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinCosFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("cos", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinCosFunction}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("tan", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTanFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("tan", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinTanFunction}));
  return Status::OK();
}

Status InitSqrtFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("sqrt", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinSqrtFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("sqrt", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinSqrtFunction}));
  return Status::OK();
}

Status InitAbsFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("abs", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinAbsFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("abs", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinAbsFunction}));

  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("abs", {ValueType::kI8}, ValueType::kI32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinAbsFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("abs", {ValueType::kI16}, ValueType::kI32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinAbsFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("abs", {ValueType::kI32}, ValueType::kI32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinAbsFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("abs", {ValueType::kI64}, ValueType::kI64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinAbsFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("abs", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinAbsFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("abs", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinAbsFunction}));
  return Status::OK();
}

Status InitCeilFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("ceil", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinCeilFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("ceil", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinCeilFunction}));
  return Status::OK();
}

Status InitFloorFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("floor", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinFloorFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("floor", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinFloorFunction}));
  return Status::OK();
}

Status InitRoundFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("round", {ValueType::kF32}, ValueType::kF32),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinRoundFunction}));
  JF_RETURN_NOT_OK(reg->RegisterFunc(FunctionSignature("round", {ValueType::kF64}, ValueType::kF64),
                                     {FunctionType::kLLVMIntrinicFunc, nullptr, CallBuiltinRoundFunction}));
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
/*
 * @Author: victorika
 * @Date: 2025-01-22 10:26:27
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-24 10:35:09
 */
#include "codegen.h"
#include "exec_node.h"
#include "function_registry.h"
#include "type.h"

namespace jitfusion {

namespace {
Status ListOrStringConcat(BinaryOPNode &binary_node, ValueType type, llvm::Value *lhs, llvm::Value *rhs,
                          IRCodeGenContext &ctx_, llvm::Value **ret_value) {
  FunctionSignature sign{type == ValueType::kString ? "StringConcat" : "ListConcat",
                         {binary_node.GetLeft()->GetReturnType(), binary_node.GetRight()->GetReturnType()},
                         ValueType::kUnknown};

  FunctionStructure func_struct;
  JF_RETURN_NOT_OK(ctx_.function_registry->GetFuncBySign(sign, &func_struct));
  *ret_value = func_struct.codegen_func(sign, {lhs->getType(), rhs->getType()}, {lhs, rhs}, ctx_);
  return Status::OK();
}

}  // namespace

Status CodeGen::SolveBinaryOpNumericType(BinaryOPNode &binary_node, llvm::Value *lhs_value, llvm::Value *rhs_value) {
  if (TypeHelper::IsRelationalBinaryOPType(binary_node.GetOp())) {  // relation Op
    auto tmp_type =
        TypeHelper::GetPromotedType(binary_node.GetLeft()->GetReturnType(), binary_node.GetRight()->GetReturnType());

    JF_RETURN_NOT_OK(NumericTypeConvert(ctx_, binary_node.GetLeft()->GetReturnType(), tmp_type, &lhs_value));
    JF_RETURN_NOT_OK(NumericTypeConvert(ctx_, binary_node.GetRight()->GetReturnType(), tmp_type, &rhs_value));

    switch (binary_node.GetOp()) {
      case BinaryOPType::kLarge: {
        value_ = TypeHelper::IsIntegerType(tmp_type) ? TypeHelper::IsSignedType(tmp_type)
                                                           ? ctx_.builder.CreateICmpSGT(lhs_value, rhs_value)
                                                           : ctx_.builder.CreateICmpUGT(lhs_value, rhs_value)
                                                     : ctx_.builder.CreateFCmpUGT(lhs_value, rhs_value);
        value_ = ctx_.builder.CreateZExt(value_, ctx_.builder.getInt8Ty());
      } break;
      case BinaryOPType::kLargeEqual: {
        value_ = TypeHelper::IsIntegerType(tmp_type) ? TypeHelper::IsSignedType(tmp_type)
                                                           ? ctx_.builder.CreateICmpSGE(lhs_value, rhs_value)
                                                           : ctx_.builder.CreateICmpUGE(lhs_value, rhs_value)
                                                     : ctx_.builder.CreateFCmpUGE(lhs_value, rhs_value);
        value_ = ctx_.builder.CreateZExt(value_, ctx_.builder.getInt8Ty());
      } break;
      case BinaryOPType::kEqual: {
        value_ = TypeHelper::IsIntegerType(tmp_type) ? ctx_.builder.CreateICmpEQ(lhs_value, rhs_value)
                                                     : ctx_.builder.CreateFCmpUEQ(lhs_value, rhs_value);
        value_ = ctx_.builder.CreateZExt(value_, ctx_.builder.getInt8Ty());
      } break;
      case BinaryOPType::kLess: {
        value_ = TypeHelper::IsIntegerType(tmp_type) ? TypeHelper::IsSignedType(tmp_type)
                                                           ? ctx_.builder.CreateICmpSLT(lhs_value, rhs_value)
                                                           : ctx_.builder.CreateICmpULT(lhs_value, rhs_value)
                                                     : ctx_.builder.CreateFCmpULT(lhs_value, rhs_value);
        value_ = ctx_.builder.CreateZExt(value_, ctx_.builder.getInt8Ty());
      } break;
      case BinaryOPType::kLessEqual: {
        value_ = TypeHelper::IsIntegerType(tmp_type) ? TypeHelper::IsSignedType(tmp_type)
                                                           ? ctx_.builder.CreateICmpSLE(lhs_value, rhs_value)
                                                           : ctx_.builder.CreateICmpULE(lhs_value, rhs_value)
                                                     : ctx_.builder.CreateFCmpULE(lhs_value, rhs_value);
        value_ = ctx_.builder.CreateZExt(value_, ctx_.builder.getInt8Ty());
      } break;
      case BinaryOPType::kNotEqual: {
        value_ = TypeHelper::IsIntegerType(tmp_type) ? ctx_.builder.CreateICmpNE(lhs_value, rhs_value)
                                                     : ctx_.builder.CreateFCmpUNE(lhs_value, rhs_value);
        value_ = ctx_.builder.CreateZExt(value_, ctx_.builder.getInt8Ty());
      } break;
      default:
        return Status::RuntimeError(
            "Unknown binary operator: ", TypeHelper::TypeToString(binary_node.GetLeft()->GetReturnType()), " ",
            TypeHelper::BinaryOPTypeToString(binary_node.GetOp()), " ",
            TypeHelper::TypeToString(binary_node.GetRight()->GetReturnType()));
    }
  } else {  // calc Op
    JF_RETURN_NOT_OK(
        NumericTypeConvert(ctx_, binary_node.GetLeft()->GetReturnType(), binary_node.GetReturnType(), &lhs_value));
    JF_RETURN_NOT_OK(
        NumericTypeConvert(ctx_, binary_node.GetRight()->GetReturnType(), binary_node.GetReturnType(), &rhs_value));

    switch (binary_node.GetOp()) {
      case BinaryOPType::kAdd: {
        value_ = TypeHelper::IsIntegerType(binary_node.GetReturnType()) ? ctx_.builder.CreateAdd(lhs_value, rhs_value)
                                                                        : ctx_.builder.CreateFAdd(lhs_value, rhs_value);
      } break;
      case BinaryOPType::kSub: {
        value_ = TypeHelper::IsIntegerType(binary_node.GetReturnType()) ? ctx_.builder.CreateSub(lhs_value, rhs_value)
                                                                        : ctx_.builder.CreateFSub(lhs_value, rhs_value);
      } break;
      case BinaryOPType::kMul: {
        value_ = TypeHelper::IsIntegerType(binary_node.GetReturnType()) ? ctx_.builder.CreateMul(lhs_value, rhs_value)
                                                                        : ctx_.builder.CreateFMul(lhs_value, rhs_value);
      } break;
      case BinaryOPType::kDiv: {
        value_ = TypeHelper::IsIntegerType(binary_node.GetReturnType())
                     ? TypeHelper::IsSignedType(binary_node.GetReturnType())
                           ? ctx_.builder.CreateSDiv(lhs_value, rhs_value)
                           : ctx_.builder.CreateUDiv(lhs_value, rhs_value)
                     : ctx_.builder.CreateFDiv(lhs_value, rhs_value);
      } break;
      case BinaryOPType::kMod: {
        value_ = TypeHelper::IsIntegerType(binary_node.GetReturnType())
                     ? TypeHelper::IsSignedType(binary_node.GetReturnType())
                           ? ctx_.builder.CreateSRem(lhs_value, rhs_value)
                           : ctx_.builder.CreateURem(lhs_value, rhs_value)
                     : ctx_.builder.CreateFRem(lhs_value, rhs_value);
      } break;
      case BinaryOPType::kAnd:
      case BinaryOPType::kBitwiseAnd: {
        value_ = ctx_.builder.CreateAnd(lhs_value, rhs_value);
      } break;
      case BinaryOPType::kOr:
      case BinaryOPType::kBitwiseOr: {
        value_ = ctx_.builder.CreateOr(lhs_value, rhs_value);
      } break;
      case BinaryOPType::kBitwiseXor: {
        value_ = ctx_.builder.CreateXor(lhs_value, rhs_value);
      } break;
      case BinaryOPType::kBitwiseShiftLeft: {
        value_ = ctx_.builder.CreateShl(lhs_value, rhs_value);
      } break;
      case BinaryOPType::kBitwiseShiftRight: {
        value_ = ctx_.builder.CreateAShr(lhs_value, rhs_value);
      } break;
      default:
        return Status::RuntimeError(
            "Unknown binary operator: ", TypeHelper::TypeToString(binary_node.GetLeft()->GetReturnType()), " ",
            TypeHelper::BinaryOPTypeToString(binary_node.GetOp()), " ",
            TypeHelper::TypeToString(binary_node.GetRight()->GetReturnType()));
    }
  }
  return Status::OK();
}

Status CodeGen::SolveBinaryOpComplexType(BinaryOPNode &binary_node, llvm::Value *lhs_value, llvm::Value *rhs_value) {
  if (BinaryOPType::kAdd == binary_node.GetOp()) {  // String or List Concat
    JF_RETURN_NOT_OK(ListOrStringConcat(binary_node, binary_node.GetReturnType(), lhs_value, rhs_value, ctx_, &value_));
  } else {  // String Compare
    if (binary_node.GetLeft()->GetReturnType() != ValueType::kString ||
        binary_node.GetRight()->GetReturnType() != ValueType::kString) {
      return Status::RuntimeError("Unknown type in StringCmp", TypeHelper::TypeToString(binary_node.GetReturnType()));
    }

    FunctionSignature sign{"StringCmp", {ValueType::kString, ValueType::kString}, ValueType::kUnknown};
    FunctionStructure func_struct;
    JF_RETURN_NOT_OK(ctx_.function_registry->GetFuncBySign(sign, &func_struct));

    llvm::FunctionCallee string_call_func_callee = ctx_.module.getOrInsertFunction(
        sign.ToString(), llvm::Type::getInt32Ty(ctx_.context), ctx_.complex_type, ctx_.complex_type);

    std::string error_info;
    llvm::raw_string_ostream error_stream(error_info);
    if (llvm::verifyFunction(llvm::cast<llvm::Function>(*string_call_func_callee.getCallee()), &error_stream)) {
      llvm::cast<llvm::Function>(*string_call_func_callee.getCallee()).print(error_stream);
      error_stream.flush();
      return Status::RuntimeError("verify function failed in binary_op_codegen in StringCmp: " + error_info);
    }

    switch (binary_node.GetOp()) {
      case BinaryOPType::kLarge: {
        value_ = ctx_.builder.CreateCall(string_call_func_callee, {lhs_value, rhs_value});
        value_ =
            ctx_.builder.CreateICmpSGT(value_, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_.context), 0, true));
      } break;
      case BinaryOPType::kLargeEqual: {
        value_ = ctx_.builder.CreateCall(string_call_func_callee, {lhs_value, rhs_value});
        value_ =
            ctx_.builder.CreateICmpSGE(value_, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_.context), 0, true));
      } break;
      case BinaryOPType::kEqual: {
        value_ = ctx_.builder.CreateCall(string_call_func_callee, {lhs_value, rhs_value});
        value_ =
            ctx_.builder.CreateICmpEQ(value_, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_.context), 0, true));
      } break;
      case BinaryOPType::kLess: {
        value_ = ctx_.builder.CreateCall(string_call_func_callee, {lhs_value, rhs_value});
        value_ =
            ctx_.builder.CreateICmpSLT(value_, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_.context), 0, true));
      } break;
      case BinaryOPType::kLessEqual: {
        value_ = ctx_.builder.CreateCall(string_call_func_callee, {lhs_value, rhs_value});
        value_ =
            ctx_.builder.CreateICmpSLE(value_, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_.context), 0, true));
      } break;
      case BinaryOPType::kNotEqual: {
        value_ = ctx_.builder.CreateCall(string_call_func_callee, {lhs_value, rhs_value});
        value_ =
            ctx_.builder.CreateICmpNE(value_, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_.context), 0, true));
      } break;
      default:
        return Status::RuntimeError(
            "Unknown binary operator: ", TypeHelper::TypeToString(binary_node.GetLeft()->GetReturnType()), " ",
            TypeHelper::BinaryOPTypeToString(binary_node.GetOp()), " ",
            TypeHelper::TypeToString(binary_node.GetRight()->GetReturnType()));
    }
    value_ = ctx_.builder.CreateZExt(value_, ctx_.builder.getInt8Ty());
  }
  return Status::OK();
}

Status CodeGen::Visit(BinaryOPNode &binary_op_node) {
  llvm::Value *lhs_value{};
  JF_RETURN_NOT_OK(GetValue(binary_op_node.GetLeft(), &lhs_value));

  llvm::Value *rhs_value{};
  JF_RETURN_NOT_OK(GetValue(binary_op_node.GetRight(), &rhs_value));

  if (TypeHelper::IsNumericType(binary_op_node.GetLeft()->GetReturnType()) &&
      TypeHelper::IsNumericType(binary_op_node.GetRight()->GetReturnType())) {
    return SolveBinaryOpNumericType(binary_op_node, lhs_value, rhs_value);
  }
  return SolveBinaryOpComplexType(binary_op_node, lhs_value, rhs_value);
}

}  // namespace jitfusion
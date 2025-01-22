/*
 * @Author: victorika
 * @Date: 2025-01-22 10:26:27
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-22 11:19:22
 */
#include "codegen.h"
#include "type.h"

namespace jitfusion {

Status CodeGen::SolveBinaryOpNumericType(BinaryOPNode &binary_node, llvm::Value *lhs_value, llvm::Value *rhs_value) {
  if (TypeHelper::IsRelationalBinaryOPType(binary_node.GetOp())) {  // relation Op
    auto tmp_type =
        TypeHelper::GetPromotedType(binary_node.GetLeft()->GetReturnType(), binary_node.GetRight()->GetReturnType());

    RETURN_NOT_OK(NumericTypeConvert(binary_node.GetLeft()->GetReturnType(), tmp_type, &lhs_value));
    RETURN_NOT_OK(NumericTypeConvert(binary_node.GetRight()->GetReturnType(), tmp_type, &rhs_value));

    switch (binary_node.GetOp()) {
      case BinaryOPType::kLarge: {
        value_ = TypeHelper::IsIntegerType(tmp_type) ? TypeHelper::IsSignedType(tmp_type)
                                                           ? ctx_.builder.CreateICmpSGT(lhs_value, rhs_value)
                                                           : ctx_.builder.CreateICmpUGT(lhs_value, rhs_value)
                                                     : ctx_.builder.CreateFCmpUGT(lhs_value, rhs_value);
        value_ = ctx_.builder.CreateSExt(value_, ctx_.builder.getInt8Ty());
      } break;
      case BinaryOPType::kLargeEqual: {
        value_ = TypeHelper::IsIntegerType(tmp_type) ? TypeHelper::IsSignedType(tmp_type)
                                                           ? ctx_.builder.CreateICmpSGE(lhs_value, rhs_value)
                                                           : ctx_.builder.CreateICmpUGE(lhs_value, rhs_value)
                                                     : ctx_.builder.CreateFCmpUGE(lhs_value, rhs_value);
        value_ = ctx_.builder.CreateSExt(value_, ctx_.builder.getInt8Ty());
      } break;
      case BinaryOPType::kEqual: {
        value_ = TypeHelper::IsIntegerType(tmp_type) ? ctx_.builder.CreateICmpEQ(lhs_value, rhs_value)
                                                     : ctx_.builder.CreateFCmpUEQ(lhs_value, rhs_value);
        value_ = ctx_.builder.CreateSExt(value_, ctx_.builder.getInt8Ty());
      } break;
      case BinaryOPType::kLess: {
        value_ = TypeHelper::IsIntegerType(tmp_type) ? TypeHelper::IsSignedType(tmp_type)
                                                           ? ctx_.builder.CreateICmpSLT(lhs_value, rhs_value)
                                                           : ctx_.builder.CreateICmpULT(lhs_value, rhs_value)
                                                     : ctx_.builder.CreateFCmpULT(lhs_value, rhs_value);
        value_ = ctx_.builder.CreateSExt(value_, ctx_.builder.getInt8Ty());
      } break;
      case BinaryOPType::kLessEqual: {
        value_ = TypeHelper::IsIntegerType(tmp_type) ? TypeHelper::IsSignedType(tmp_type)
                                                           ? ctx_.builder.CreateICmpSLE(lhs_value, rhs_value)
                                                           : ctx_.builder.CreateICmpULE(lhs_value, rhs_value)
                                                     : ctx_.builder.CreateFCmpULE(lhs_value, rhs_value);
        value_ = ctx_.builder.CreateSExt(value_, ctx_.builder.getInt8Ty());
      } break;
      case BinaryOPType::kNotEqual: {
        value_ = TypeHelper::IsIntegerType(tmp_type) ? ctx_.builder.CreateICmpNE(lhs_value, rhs_value)
                                                     : ctx_.builder.CreateFCmpUNE(lhs_value, rhs_value);
        value_ = ctx_.builder.CreateSExt(value_, ctx_.builder.getInt8Ty());
      } break;
      default:
        return Status::RuntimeError(
            "Unknown binary operator: ", TypeHelper::TypeToString(binary_node.GetLeft()->GetReturnType()), " ",
            TypeHelper::BinaryOPTypeToString(binary_node.GetOp()), " ",
            TypeHelper::TypeToString(binary_node.GetRight()->GetReturnType()));
    }
  } else {  // calc Op
    RETURN_NOT_OK(NumericTypeConvert(binary_node.GetLeft()->GetReturnType(), binary_node.GetReturnType(), &lhs_value));
    RETURN_NOT_OK(NumericTypeConvert(binary_node.GetRight()->GetReturnType(), binary_node.GetReturnType(), &rhs_value));

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
    // TODO(victorika):
  } else {  // String Compare
    if (binary_node.GetLeft()->GetReturnType() != ValueType::kString ||
        binary_node.GetRight()->GetReturnType() != ValueType::kString) {
      return Status::RuntimeError("Unknown type in StringCmp", TypeHelper::TypeToString(binary_node.GetReturnType()));
    }
    // TODO(victorika):
  }
  return Status::OK();
}

Status CodeGen::Visit(BinaryOPNode &binary_op_node) {
  llvm::Value *lhs_value{};
  RETURN_NOT_OK(GetValue(binary_op_node.GetLeft(), &lhs_value));

  llvm::Value *rhs_value{};
  RETURN_NOT_OK(GetValue(binary_op_node.GetRight(), &rhs_value));

  if (TypeHelper::IsNumberType(binary_op_node.GetLeft()->GetReturnType()) &&
      TypeHelper::IsNumberType(binary_op_node.GetRight()->GetReturnType())) {
    return SolveBinaryOpNumericType(binary_op_node, lhs_value, rhs_value);
  }
  return SolveBinaryOpComplexType(binary_op_node, lhs_value, rhs_value);
}

}  // namespace jitfusion
/*
 * @Author: victorika
 * @Date: 2025-01-21 17:00:50
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-22 11:18:28
 */
#include "codegen.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

Status CodeGen::Visit(UnaryOPNode &unary_op_node) {
  llvm::Value *child_value{};
  RETURN_NOT_OK(GetValue(unary_op_node.GetChild(), &child_value));

  RETURN_NOT_OK(
      NumericTypeConvert(ctx_, unary_op_node.GetChild()->GetReturnType(), unary_op_node.GetReturnType(), &child_value));
  switch (unary_op_node.GetOp()) {
    case UnaryOPType::kPlus: {
      value_ = child_value;
    } break;
    case UnaryOPType::kMinus: {
      value_ = TypeHelper::IsIntegerType(unary_op_node.GetReturnType()) ? ctx_.builder.CreateNeg(child_value, "neg")
                                                                        : ctx_.builder.CreateFNeg(child_value, "neg");
    } break;
    case UnaryOPType::kNot: {
      auto *false_value = llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx_.context), 0, false);
      value_ = ctx_.builder.CreateICmpEQ(child_value, false_value, "not_x_icmp");
      value_ = ctx_.builder.CreateZExt(value_, ctx_.builder.getInt8Ty());
    } break;
    case UnaryOPType::kBitwiseNot: {
      value_ = ctx_.builder.CreateNot(child_value, "bitwise_not");
    } break;
    default:
      return Status::RuntimeError("Unknown unary operator: ", TypeHelper::UnaryOPTypeToString(unary_op_node.GetOp()));
  }
  return Status::OK();
}

}  // namespace jitfusion
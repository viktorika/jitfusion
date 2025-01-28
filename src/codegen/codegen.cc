/*
 * @Author: victorika
 * @Date: 2025-01-20 14:47:10
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-28 21:52:50
 */
#include "codegen.h"
#include <type.h>
#include "exec_node.h"

namespace jitfusion {

Status CodeGen::GetValue(ExecNode *node, llvm::Value **ret_value) {
  RETURN_NOT_OK(node->Accept(this));
  *ret_value = value_;
  return Status::OK();
}

Status CodeGen::ToBoolean(ValueType from, llvm::Value **value) {
  switch (from) {
    case ValueType::kU8:
      return Status::OK();
    case ValueType::kI8:
    case ValueType::kI16:
    case ValueType::kI32:
    case ValueType::kI64: {
      llvm::Value *condition = ctx_.builder.CreateICmpNE(*value, llvm::ConstantInt::get((*value)->getType(), 0, true));
      *value = ctx_.builder.CreateSExt(condition, llvm::Type::getInt8Ty(ctx_.context));
    } break;
    case ValueType::kU16:
    case ValueType::kU32:
    case ValueType::kU64: {
      llvm::Value *condition = ctx_.builder.CreateICmpNE(*value, llvm::ConstantInt::get((*value)->getType(), 0, false));
      *value = ctx_.builder.CreateSExt(condition, llvm::Type::getInt8Ty(ctx_.context));
    } break;
    case ValueType::kF32:
    case ValueType::kF64: {
      llvm::Value *condition = ctx_.builder.CreateFCmpUNE(*value, llvm::ConstantFP::get((*value)->getType(), 0));
      *value = ctx_.builder.CreateSExt(condition, llvm::Type::getInt8Ty(ctx_.context));
    } break;
    default:
      return Status::RuntimeError("Unknown convert in ToBoolean, from ", TypeHelper::TypeToString(from), " to uint8");
  }
  return Status::OK();
}

Status CodeGen::NumericTypeConvert(ValueType from, ValueType to, llvm::Value **value) {
  if (from == to) {
    return Status::OK();
  }

  if (to == ValueType::kU8) {
    return ToBoolean(from, value);
  }
  if (TypeHelper::IsIntegerType(from)) {
    if (TypeHelper::IsIntegerType(to)) {
      *value =
          ctx_.builder.CreateIntCast(*value, llvm::IntegerType::get(ctx_.context, TypeHelper::GetBitWidthFromType(to)),
                                     TypeHelper::IsSignedType(to));
      return Status::OK();
    }
    if (TypeHelper::IsFloatType(to)) {
      if (TypeHelper::IsSignedType(from)) {
        *value = (to == ValueType::kF32 ? ctx_.builder.CreateSIToFP(*value, llvm::Type::getFloatTy(ctx_.context))
                                        : ctx_.builder.CreateSIToFP(*value, llvm::Type::getDoubleTy(ctx_.context)));
      } else {
        *value = (to == ValueType::kF32 ? ctx_.builder.CreateUIToFP(*value, llvm::Type::getFloatTy(ctx_.context))
                                        : ctx_.builder.CreateUIToFP(*value, llvm::Type::getDoubleTy(ctx_.context)));
      }
      return Status::OK();
    }
  } else if (TypeHelper::IsFloatType(from)) {
    if (TypeHelper::IsIntegerType(to)) {
      if (TypeHelper::IsSignedType(to)) {
        *value = ctx_.builder.CreateFPToSI(*value,
                                           llvm::IntegerType::get(ctx_.context, TypeHelper::GetBitWidthFromType(to)));
      } else {
        *value = ctx_.builder.CreateFPToUI(*value,
                                           llvm::IntegerType::get(ctx_.context, TypeHelper::GetBitWidthFromType(to)));
      }
      return Status::OK();
    }
    if (TypeHelper::IsFloatType(to)) {
      if (TypeHelper::GetBitWidthFromType(from) < TypeHelper::GetBitWidthFromType(to)) {
        *value = ctx_.builder.CreateFPExt(*value, llvm::Type::getDoubleTy(ctx_.context));
      } else if (TypeHelper::GetBitWidthFromType(from) > TypeHelper::GetBitWidthFromType(to)) {
        *value = ctx_.builder.CreateFPTrunc(*value, llvm::Type::getFloatTy(ctx_.context));
      }
      return Status::OK();
    }
  }
  return Status::RuntimeError("Unknown convert in TypeConvert, from ", TypeHelper::TypeToString(from), " to ",
                              TypeHelper::TypeToString(to));
}

}  // namespace jitfusion
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

Status CodeGen::NumericTypeConvert(IRCodeGenContext &ctx, ValueType from, ValueType to, llvm::Value **value) {
  if (from == to) {
    return Status::OK();
  }

  if (TypeHelper::IsIntegerType(from)) {
    if (TypeHelper::IsIntegerType(to)) {
      *value =
          ctx.builder.CreateIntCast(*value, llvm::IntegerType::get(ctx.context, TypeHelper::GetBitWidthFromType(to)),
                                    TypeHelper::IsSignedType(to));
      return Status::OK();
    }
    if (TypeHelper::IsFloatType(to)) {
      if (TypeHelper::IsSignedType(from)) {
        *value = (to == ValueType::kF32 ? ctx.builder.CreateSIToFP(*value, llvm::Type::getFloatTy(ctx.context))
                                        : ctx.builder.CreateSIToFP(*value, llvm::Type::getDoubleTy(ctx.context)));
      } else {
        *value = (to == ValueType::kF32 ? ctx.builder.CreateUIToFP(*value, llvm::Type::getFloatTy(ctx.context))
                                        : ctx.builder.CreateUIToFP(*value, llvm::Type::getDoubleTy(ctx.context)));
      }
      return Status::OK();
    }
  } else if (TypeHelper::IsFloatType(from)) {
    if (TypeHelper::IsIntegerType(to)) {
      if (TypeHelper::IsSignedType(to)) {
        *value =
            ctx.builder.CreateFPToSI(*value, llvm::IntegerType::get(ctx.context, TypeHelper::GetBitWidthFromType(to)));
      } else {
        *value =
            ctx.builder.CreateFPToUI(*value, llvm::IntegerType::get(ctx.context, TypeHelper::GetBitWidthFromType(to)));
      }
      return Status::OK();
    }
    if (TypeHelper::IsFloatType(to)) {
      if (TypeHelper::GetBitWidthFromType(from) < TypeHelper::GetBitWidthFromType(to)) {
        *value = ctx.builder.CreateFPExt(*value, llvm::Type::getDoubleTy(ctx.context));
      } else if (TypeHelper::GetBitWidthFromType(from) > TypeHelper::GetBitWidthFromType(to)) {
        *value = ctx.builder.CreateFPTrunc(*value, llvm::Type::getFloatTy(ctx.context));
      }
      return Status::OK();
    }
  }
  return Status::RuntimeError("Unknown convert in TypeConvert, from ", TypeHelper::TypeToString(from), " to ",
                              TypeHelper::TypeToString(to));
}

}  // namespace jitfusion
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
  JF_RETURN_NOT_OK(node->Accept(this));
  *ret_value = value_;
  return Status::OK();
}

Status CodeGen::ValueTypeToLLVMType(IRCodeGenContext &ctx, ValueType value_type, llvm::Type **llvm_type) {
  switch (value_type) {
    case ValueType::kU8:
    case ValueType::kI8: {
      *llvm_type = llvm::Type::getInt8Ty(ctx.context);
    } break;
    case ValueType::kU16:
    case ValueType::kI16: {
      *llvm_type = llvm::Type::getInt16Ty(ctx.context);
    } break;
    case ValueType::kU32:
    case ValueType::kI32: {
      *llvm_type = llvm::Type::getInt32Ty(ctx.context);
    } break;
    case ValueType::kU64:
    case ValueType::kI64: {
      *llvm_type = llvm::Type::getInt64Ty(ctx.context);
    } break;
    case ValueType::kF32: {
      *llvm_type = llvm::Type::getFloatTy(ctx.context);
    } break;
    case ValueType::kF64: {
      *llvm_type = llvm::Type::getDoubleTy(ctx.context);
    } break;
    case ValueType::kString:
    case ValueType::kU8List:
    case ValueType::kI8List:
    case ValueType::kU16List:
    case ValueType::kI16List:
    case ValueType::kU32List:
    case ValueType::kI32List:
    case ValueType::kU64List:
    case ValueType::kI64List:
    case ValueType::kF32List:
    case ValueType::kF64List:
    case ValueType::kStringList: {
      *llvm_type = ctx.complex_type;
    } break;
    default:
      return Status::RuntimeError("ValueType ", TypeHelper::TypeToString(value_type), " ",
                                  "can not convert to llvm type");
  }
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
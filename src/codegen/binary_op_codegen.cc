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
  FunctionSignature sign{
      type == ValueType::kString ? "StringConcat" : "ListConcat",
      {binary_node.GetLeft()->GetReturnType(), binary_node.GetRight()->GetReturnType(), ValueType::kPtr},
      ValueType::kUnknown};

  FunctionStructure func_struct;
  JF_RETURN_NOT_OK(ctx_.function_registry->GetFuncBySign(sign, &func_struct));
  llvm::Type *llvm_ret_type;
  JF_RETURN_NOT_OK(CodeGen::ValueTypeToLLVMType(ctx_, sign.GetRetType(), &llvm_ret_type));
  llvm::FunctionType *func_type = llvm::FunctionType::get(llvm_ret_type,
                                                          {
                                                              lhs->getType(),
                                                              rhs->getType(),
                                                              llvm::Type::getVoidTy(ctx_.context)->getPointerTo(),
                                                          },
                                                          false);
  llvm::FunctionCallee add_func_callee;
  add_func_callee = ctx_.module.getOrInsertFunction(sign.ToString(), func_type);

  std::string error_info;
  llvm::raw_string_ostream error_stream(error_info);
  if (llvm::verifyFunction(llvm::cast<llvm::Function>(*add_func_callee.getCallee()), &error_stream)) {
    llvm::cast<llvm::Function>(*add_func_callee.getCallee()).print(error_stream);
    error_stream.flush();
    return Status::RuntimeError("verify function failed in binary_op_codegen in add_func_callee: " + error_info);
  }

  *ret_value = ctx_.builder.CreateCall(add_func_callee, {lhs, rhs, ctx_.entry_function->getArg(1)}, "calltmp");
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
      case BinaryOPType::kBitwiseAnd: {
        value_ = ctx_.builder.CreateAnd(lhs_value, rhs_value);
      } break;
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

    llvm::FunctionCallee string_call_func_callee =
        ctx_.module.getOrInsertFunction(sign.ToString(), llvm::Type::getInt32Ty(ctx_.context),
                                        ctx_.complex_type.string_type, ctx_.complex_type.string_type);

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

Status CodeGen::SolveShortCircuitLogicalOp(BinaryOPNode &binary_op_node) {
  bool is_and = (binary_op_node.GetOp() == BinaryOPType::kAnd);

  llvm::Function *cur_function = ctx_.entry_function;
  llvm::BasicBlock *eval_rhs_block = llvm::BasicBlock::Create(ctx_.context, "sc.rhs", cur_function);
  llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(ctx_.context, "sc.merge", cur_function);

  llvm::Value *lhs_value{};
  JF_RETURN_NOT_OK(GetValue(binary_op_node.GetLeft(), &lhs_value));

  llvm::Value *lhs_bool{};
  if (lhs_value->getType()->isIntegerTy()) {
    lhs_bool = ctx_.builder.CreateICmpNE(lhs_value, llvm::ConstantInt::get(lhs_value->getType(), 0), "lhs.tobool");
  } else if (lhs_value->getType()->isFloatingPointTy()) {
    lhs_bool = ctx_.builder.CreateFCmpONE(lhs_value, llvm::ConstantFP::get(lhs_value->getType(), 0.0), "lhs.tobool");
  } else {
    return Status::RuntimeError("Unsupported type for logical operator");
  }

  llvm::BasicBlock *lhs_end_block = ctx_.builder.GetInsertBlock();

  if (is_and) {
    ctx_.builder.CreateCondBr(lhs_bool, eval_rhs_block, merge_block);
  } else {
    ctx_.builder.CreateCondBr(lhs_bool, merge_block, eval_rhs_block);
  }

  ctx_.builder.SetInsertPoint(eval_rhs_block);
  llvm::Value *rhs_value{};
  JF_RETURN_NOT_OK(GetValue(binary_op_node.GetRight(), &rhs_value));

  llvm::Value *rhs_bool{};
  if (rhs_value->getType()->isIntegerTy()) {
    rhs_bool = ctx_.builder.CreateICmpNE(rhs_value, llvm::ConstantInt::get(rhs_value->getType(), 0), "rhs.tobool");
  } else if (rhs_value->getType()->isFloatingPointTy()) {
    rhs_bool = ctx_.builder.CreateFCmpONE(rhs_value, llvm::ConstantFP::get(rhs_value->getType(), 0.0), "rhs.tobool");
  } else {
    return Status::RuntimeError("Unsupported type for logical operator");
  }

  llvm::BasicBlock *rhs_end_block = ctx_.builder.GetInsertBlock();
  ctx_.builder.CreateBr(merge_block);

  ctx_.builder.SetInsertPoint(merge_block);
  llvm::PHINode *phi = ctx_.builder.CreatePHI(ctx_.builder.getInt1Ty(), 2, "sc.phi");

  if (is_and) {
    phi->addIncoming(llvm::ConstantInt::getFalse(ctx_.context), lhs_end_block);
    phi->addIncoming(rhs_bool, rhs_end_block);
  } else {
    phi->addIncoming(llvm::ConstantInt::getTrue(ctx_.context), lhs_end_block);
    phi->addIncoming(rhs_bool, rhs_end_block);
  }
  value_ = ctx_.builder.CreateZExt(phi, ctx_.builder.getInt8Ty());
  return Status::OK();
}

Status CodeGen::Visit(BinaryOPNode &binary_op_node) {
  if (TypeHelper::IsLogicalBinaryOPType(binary_op_node.GetOp())) {
    return SolveShortCircuitLogicalOp(binary_op_node);
  }

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
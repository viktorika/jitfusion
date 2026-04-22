/*
 * @Author: victorika
 * @Date: 2025-01-23 15:09:09
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-08 14:21:00
 */
#include "codegen.h"
#include "type.h"

namespace jitfusion {

Status CodeGen::Visit(IfNode &if_node) {
  llvm::Type *ret_type_llvm;
  JF_RETURN_NOT_OK(ValueTypeToLLVMType(ctx_, if_node.GetReturnType(), &ret_type_llvm));

  llvm::Value *cond_value{};
  JF_RETURN_NOT_OK(GetValue(if_node.GetArgs()[0].get(), &cond_value));
  if (cond_value->getType()->isIntegerTy()) {
    cond_value = ctx_.builder.CreateICmpNE(cond_value, llvm::ConstantInt::get(cond_value->getType(), 0), "to_bool");
  } else if (cond_value->getType()->isFloatingPointTy()) {
    cond_value = ctx_.builder.CreateFCmpONE(cond_value, llvm::ConstantFP::get(cond_value->getType(), 0.0), "to_bool");
  } else {
    return Status::RuntimeError("[internal] unsupported type for if condition (should be caught by validator)");
  }

  llvm::Function *cur_function = ctx_.entry_function;
  llvm::BasicBlock *then_block = llvm::BasicBlock::Create(ctx_.context, "then", cur_function);
  llvm::BasicBlock *else_block = llvm::BasicBlock::Create(ctx_.context, "else", cur_function);
  llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(ctx_.context, "merge", cur_function);
  ctx_.builder.CreateCondBr(cond_value, then_block, else_block);

  ctx_.builder.SetInsertPoint(then_block);
  llvm::Value *then_value{};
  JF_RETURN_NOT_OK(GetValue(if_node.GetArgs()[1].get(), &then_value));
  JF_RETURN_NOT_OK(
      NumericTypeConvert(ctx_, if_node.GetArgs()[1]->GetReturnType(), if_node.GetReturnType(), &then_value));

  llvm::BasicBlock *then_end_block = ctx_.builder.GetInsertBlock();
  ctx_.builder.CreateBr(merge_block);

  ctx_.builder.SetInsertPoint(else_block);
  llvm::Value *else_value{};
  JF_RETURN_NOT_OK(GetValue(if_node.GetArgs()[2].get(), &else_value));
  JF_RETURN_NOT_OK(
      NumericTypeConvert(ctx_, if_node.GetArgs()[2]->GetReturnType(), if_node.GetReturnType(), &else_value));
  llvm::BasicBlock *else_end_block = ctx_.builder.GetInsertBlock();
  ctx_.builder.CreateBr(merge_block);

  ctx_.builder.SetInsertPoint(merge_block);
  llvm::PHINode *phi = ctx_.builder.CreatePHI(ret_type_llvm, 2, "phi");
  phi->addIncoming(then_value, then_end_block);
  phi->addIncoming(else_value, else_end_block);

  value_ = phi;
  return Status::OK();
}

}  // namespace jitfusion
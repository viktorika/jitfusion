/*
 * @Author: victorika
 * @Date: 2025-01-23 16:00:20
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-08 14:21:00
 */
#include "codegen.h"
#include "type.h"

namespace jitfusion {

Status CodeGen::Visit(SwitchNode &switch_node) {
  llvm::Type *ret_type_llvm;
  JF_RETURN_NOT_OK(ValueTypeToLLVMType(ctx_, switch_node.GetReturnType(), &ret_type_llvm));

  llvm::Function *cur_function = ctx_.entry_function;
  llvm::BasicBlock *original_block = ctx_.builder.GetInsertBlock();

  llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(ctx_.context, "switch.merge", cur_function);

  std::vector<std::pair<llvm::Value *, llvm::BasicBlock *>> incoming_values;

  const auto &args = switch_node.GetArgs();
  int num_args = static_cast<int>(args.size());

  llvm::BasicBlock *default_block = llvm::BasicBlock::Create(ctx_.context, "switch.default", cur_function);
  ctx_.builder.SetInsertPoint(default_block);
  llvm::Value *default_value{};
  JF_RETURN_NOT_OK(GetValue(args[num_args - 1].get(), &default_value));
  JF_RETURN_NOT_OK(
      NumericTypeConvert(ctx_, args[num_args - 1]->GetReturnType(), switch_node.GetReturnType(), &default_value));
  llvm::BasicBlock *default_end_block = ctx_.builder.GetInsertBlock();
  ctx_.builder.CreateBr(merge_block);
  incoming_values.emplace_back(default_value, default_end_block);

  llvm::BasicBlock *prev_false_block = default_block;

  for (int i = num_args - 3; i >= 0; i -= 2) {
    llvm::BasicBlock *cond_block = llvm::BasicBlock::Create(ctx_.context, "switch.cond", cur_function);
    ctx_.builder.SetInsertPoint(cond_block);

    llvm::Value *cond_value{};
    JF_RETURN_NOT_OK(GetValue(args[i].get(), &cond_value));
    if (cond_value->getType()->isIntegerTy()) {
      cond_value = ctx_.builder.CreateICmpNE(cond_value, llvm::ConstantInt::get(cond_value->getType(), 0), "to_bool");
    } else if (cond_value->getType()->isFloatingPointTy()) {
      cond_value = ctx_.builder.CreateFCmpONE(cond_value, llvm::ConstantFP::get(cond_value->getType(), 0.0), "to_bool");
    } else {
      return Status::RuntimeError("[internal] unsupported type for switch condition (should be caught by validator)");
    }

    llvm::BasicBlock *then_block = llvm::BasicBlock::Create(ctx_.context, "switch.then", cur_function);
    ctx_.builder.CreateCondBr(cond_value, then_block, prev_false_block);

    ctx_.builder.SetInsertPoint(then_block);
    llvm::Value *then_value{};
    JF_RETURN_NOT_OK(GetValue(args[i + 1].get(), &then_value));
    JF_RETURN_NOT_OK(NumericTypeConvert(ctx_, args[i + 1]->GetReturnType(), switch_node.GetReturnType(), &then_value));
    llvm::BasicBlock *then_end_block = ctx_.builder.GetInsertBlock();
    ctx_.builder.CreateBr(merge_block);
    incoming_values.emplace_back(then_value, then_end_block);

    prev_false_block = cond_block;
  }

  ctx_.builder.SetInsertPoint(original_block);
  ctx_.builder.CreateBr(prev_false_block);

  ctx_.builder.SetInsertPoint(merge_block);
  llvm::PHINode *phi = ctx_.builder.CreatePHI(ret_type_llvm, static_cast<unsigned>(incoming_values.size()), "phi");
  for (const auto &[val, block] : incoming_values) {
    phi->addIncoming(val, block);
  }

  value_ = phi;
  return Status::OK();
}

}  // namespace jitfusion
/*
 * @Author: victorika
 * @Date: 2025-01-23 16:00:20
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-30 22:36:45
 */
#include "codegen.h"
#include "type.h"

namespace jitfusion {

Status CodeGen::Visit(SwitchNode &switch_node) {
  llvm::Type *ret_type_llvm;
  JF_RETURN_NOT_OK(ValueTypeToLLVMType(switch_node.GetReturnType(), &ret_type_llvm));

  llvm::BasicBlock *cur_block = ctx_.entry_bb;
  llvm::Function *cur_function = ctx_.entry_function;

  llvm::FunctionType *func_type = llvm::FunctionType::get(
      ret_type_llvm,
      llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt64Ty(ctx_.context), llvm::Type::getInt64Ty(ctx_.context)}),
      false);

  llvm::Function *switch_func =
      llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, "switch", ctx_.module);

  llvm::BasicBlock *switch_entry_block = llvm::BasicBlock::Create(ctx_.context, "switch", switch_func);
  llvm::BasicBlock *prev_block = nullptr;
  for (int i = switch_node.GetArgs().size() - 1; i >= 0; i -= 2) {
    llvm::BasicBlock *switch_block = llvm::BasicBlock::Create(ctx_.context, "switch", switch_func);
    ctx_.entry_bb = switch_block;
    ctx_.entry_function = switch_func;
    ctx_.builder.SetInsertPoint(switch_block);
    llvm::Value *cond_value;
    llvm::Value *tmp_value;
    if (prev_block == nullptr) {
      JF_RETURN_NOT_OK(GetValue(switch_node.GetArgs()[i].get(), &tmp_value));
      NumericTypeConvert(ctx_, switch_node.GetArgs()[i]->GetReturnType(), switch_node.GetReturnType(), &tmp_value);
      ctx_.builder.CreateRet(tmp_value);
    } else {
      JF_RETURN_NOT_OK(GetValue(switch_node.GetArgs()[i].get(), &cond_value));
      NumericTypeConvert(ctx_, switch_node.GetArgs()[i]->GetReturnType(), ValueType::kU8, &cond_value);
      cond_value = ctx_.builder.CreateTruncOrBitCast(cond_value, llvm::Type::getInt1Ty(ctx_.context), "i1");

      llvm::BasicBlock *then_block = llvm::BasicBlock::Create(ctx_.context, "then", switch_func);

      ctx_.builder.CreateCondBr(cond_value, then_block, prev_block);

      ctx_.entry_bb = then_block;
      ctx_.entry_function = switch_func;
      ctx_.builder.SetInsertPoint(then_block);
      JF_RETURN_NOT_OK(GetValue(switch_node.GetArgs()[i + 1].get(), &tmp_value));
      NumericTypeConvert(ctx_, switch_node.GetArgs()[i + 1]->GetReturnType(), switch_node.GetReturnType(), &tmp_value);
      ctx_.builder.CreateRet(tmp_value);
    }
    prev_block = switch_block;
  }
  ctx_.entry_bb = switch_entry_block;
  ctx_.entry_function = switch_func;
  ctx_.builder.SetInsertPoint(switch_entry_block);
  ctx_.builder.CreateBr(prev_block);

  ctx_.entry_bb = cur_block;
  ctx_.entry_function = cur_function;
  ctx_.builder.SetInsertPoint(cur_block);

  std::string error_info;
  llvm::raw_string_ostream error_stream(error_info);
  if (llvm::verifyFunction(*switch_func, &error_stream)) {
    switch_func->print(error_stream);
    error_stream.flush();
    return Status::RuntimeError("verify function failed in function_codegen in switch_func: " + error_info);
  }

  value_ =
      ctx_.builder.CreateCall(switch_func, {ctx_.entry_function->getArg(0), ctx_.entry_function->getArg(1)}, "switch");
  return Status::OK();
}

}  // namespace jitfusion
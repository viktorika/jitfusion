/*
 * @Author: victorika
 * @Date: 2025-01-23 15:09:09
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-23 16:02:02
 */
#include "codegen.h"
#include "type.h"

namespace jitfusion {

Status CodeGen::Visit(IfNode &if_node) {
  llvm::Type *ret_type_llvm;
  RETURN_NOT_OK(ValueTypeToLLVMType(if_node.GetReturnType(), &ret_type_llvm));

  llvm::BasicBlock *cur_block = ctx_.entry_bb;
  llvm::Function *cur_function = ctx_.entry_function;

  llvm::FunctionType *func_type = llvm::FunctionType::get(
      ret_type_llvm,
      llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt64Ty(ctx_.context), llvm::Type::getInt64Ty(ctx_.context)}),
      false);

  llvm::Function *if_func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, "if", ctx_.module);
  llvm::BasicBlock *if_block = llvm::BasicBlock::Create(ctx_.context, "if", if_func);

  ctx_.entry_bb = if_block;
  ctx_.entry_function = if_func;
  ctx_.builder.SetInsertPoint(if_block);
  llvm::Value *cond_value{};
  RETURN_NOT_OK(GetValue(if_node.GetArgs()[0].get(), &cond_value));
  cond_value = ctx_.builder.CreateTruncOrBitCast(cond_value, llvm::Type::getInt1Ty(ctx_.context), "i1");

  // 创建 if-else 结构
  llvm::BasicBlock *then_block = llvm::BasicBlock::Create(ctx_.context, "then", if_func);
  llvm::BasicBlock *else_block = llvm::BasicBlock::Create(ctx_.context, "else", if_func);
  llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(ctx_.context, "merge", if_func);
  ctx_.builder.CreateCondBr(cond_value, then_block, else_block);

  // 创建 then 块
  ctx_.entry_bb = then_block;
  ctx_.entry_function = if_func;
  ctx_.builder.SetInsertPoint(then_block);
  llvm::Value *then_value{};
  RETURN_NOT_OK(GetValue(if_node.GetArgs()[1].get(), &then_value));
  NumericTypeConvert(if_node.GetArgs()[1]->GetReturnType(), if_node.GetReturnType(), &then_value);
  ctx_.builder.CreateBr(merge_block);

  // 创建 else 块
  ctx_.entry_bb = else_block;
  ctx_.entry_function = if_func;
  ctx_.builder.SetInsertPoint(else_block);
  llvm::Value *else_value{};
  RETURN_NOT_OK(GetValue(if_node.GetArgs()[2].get(), &else_value));
  NumericTypeConvert(if_node.GetArgs()[2]->GetReturnType(), if_node.GetReturnType(), &else_value);
  ctx_.builder.CreateBr(merge_block);

  // 创建 merge 块
  ctx_.entry_bb = merge_block;
  ctx_.entry_function = if_func;
  ctx_.builder.SetInsertPoint(merge_block);
  llvm::PHINode *phi = ctx_.builder.CreatePHI(ret_type_llvm, 2, "phi");
  phi->addIncoming(then_value, then_block);
  phi->addIncoming(else_value, else_block);
  ctx_.builder.CreateRet(phi);

  // 恢复 entry_bb, entry_function
  ctx_.entry_bb = cur_block;
  ctx_.entry_function = cur_function;
  ctx_.builder.SetInsertPoint(cur_block);

  std::string error_info;
  llvm::raw_string_ostream error_stream(error_info);
  if (llvm::verifyFunction(*if_func, &error_stream)) {
    if_func->print(error_stream);
    error_stream.flush();
    return Status::RuntimeError("verify function failed in function_codegen in if_func: " + error_info);
  }

  value_ = ctx_.builder.CreateCall(if_func, {ctx_.entry_function->getArg(0), ctx_.entry_function->getArg(1)}, "if(i1)");

  return Status::OK();
}

}  // namespace jitfusion
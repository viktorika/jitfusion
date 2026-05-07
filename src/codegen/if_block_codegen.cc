/*
 * @Author: victorika
 * @Date: 2026-04-17 12:30:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-17 16:00:00
 */
#include <unordered_set>
#include "codegen.h"
#include "type.h"

namespace jitfusion {

Status CodeGen::Visit(IfBlockNode &if_block_node) {
  llvm::Function *cur_function = ctx_.entry_function;
  llvm::BasicBlock *original_block = ctx_.builder.GetInsertBlock();
  llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(ctx_.context, "ifblock.merge", cur_function);

  const auto &args = if_block_node.GetArgs();
  int num_args = static_cast<int>(args.size());
  bool has_else = (num_args % 2 != 0);
  int num_branches = num_args / 2;

  struct BranchInfo {
    llvm::BasicBlock *end_block;
    std::unordered_map<std::string, llvm::Value *> modified;
    BranchInfo(llvm::BasicBlock *block, std::unordered_map<std::string, llvm::Value *> mod)
        : end_block(block), modified(std::move(mod)) {}
  };
  std::vector<BranchInfo> branch_infos;

  llvm::BasicBlock *fallthrough_block = merge_block;
  if (has_else) {
    llvm::BasicBlock *else_block = llvm::BasicBlock::Create(ctx_.context, "ifblock.else", cur_function);
    ctx_.builder.SetInsertPoint(else_block);
    // Branch scope isolation: PushScope creates a new scope so that modifications
    // to outer variables within this branch are recorded separately. After executing
    // the branch body, GetShadowed() collects the modified variables before PopScope
    // discards them. The collected values are later merged via PHI nodes at the
    // merge block.
    // NOTE: This scope operation differs from NoOPNode's isolated scope, which is
    // used for variable isolation between independent child groups. The two can
    // safely nest without interference.
    scope_stack_.PushScope();
    llvm::Value *else_value;
    JF_RETURN_NOT_OK(GetValue(args[num_args - 1].get(), &else_value));
    auto modified = scope_stack_.GetShadowed();
    llvm::BasicBlock *else_end_block = ctx_.builder.GetInsertBlock();
    scope_stack_.PopScope();
    ctx_.builder.CreateBr(merge_block);
    branch_infos.emplace_back(else_end_block, std::move(modified));
    fallthrough_block = else_block;
  }

  llvm::BasicBlock *prev_false_block = fallthrough_block;
  llvm::BasicBlock *last_cond_block = nullptr;

  for (int i = num_branches - 1; i >= 0; --i) {
    int cond_idx = i * 2;
    int body_idx = (i * 2) + 1;

    llvm::BasicBlock *cond_block = llvm::BasicBlock::Create(ctx_.context, "ifblock.cond", cur_function);
    ctx_.builder.SetInsertPoint(cond_block);

    if (last_cond_block == nullptr) {
      last_cond_block = cond_block;
    }

    llvm::Value *cond_value{};
    JF_RETURN_NOT_OK(GetValue(args[cond_idx].get(), &cond_value));
    if (cond_value->getType()->isIntegerTy()) {
      cond_value = ctx_.builder.CreateICmpNE(cond_value, llvm::ConstantInt::get(cond_value->getType(), 0), "to_bool");
    } else if (cond_value->getType()->isFloatingPointTy()) {
      cond_value = ctx_.builder.CreateFCmpONE(cond_value, llvm::ConstantFP::get(cond_value->getType(), 0.0), "to_bool");
    } else {
      return Status::RuntimeError("[internal] unsupported type for if block condition (should be caught by validator)");
    }

    llvm::BasicBlock *body_block = llvm::BasicBlock::Create(ctx_.context, "ifblock.body", cur_function);
    ctx_.builder.CreateCondBr(cond_value, body_block, prev_false_block);

    ctx_.builder.SetInsertPoint(body_block);
    // Branch scope isolation: same as the else branch above — PushScope, execute
    // body, GetShadowed to collect modifications, then PopScope. The collected
    // modified variables will be merged via PHI nodes at the merge block.
    scope_stack_.PushScope();
    llvm::Value *body_value;
    JF_RETURN_NOT_OK(GetValue(args[body_idx].get(), &body_value));
    auto modified = scope_stack_.GetShadowed();
    llvm::BasicBlock *body_end_block = ctx_.builder.GetInsertBlock();
    scope_stack_.PopScope();
    ctx_.builder.CreateBr(merge_block);
    branch_infos.emplace_back(body_end_block, std::move(modified));

    prev_false_block = cond_block;
  }

  ctx_.builder.SetInsertPoint(original_block);
  ctx_.builder.CreateBr(prev_false_block);

  ctx_.builder.SetInsertPoint(merge_block);

  std::unordered_set<std::string> all_modified_vars;
  for (const auto &info : branch_infos) {
    for (const auto &[name, val] : info.modified) {
      all_modified_vars.insert(name);
    }
  }

  if (!all_modified_vars.empty()) {
    auto num_incoming = branch_infos.size();
    if (!has_else) {
      num_incoming += 1;
    }

    for (const auto &var_name : all_modified_vars) {
      llvm::Value *original_value = scope_stack_.Lookup(var_name);
      llvm::PHINode *phi = ctx_.builder.CreatePHI(original_value->getType(), num_incoming, var_name + ".phi");
      for (const auto &info : branch_infos) {
        auto it = info.modified.find(var_name);
        if (it != info.modified.end()) {
          phi->addIncoming(it->second, info.end_block);
        } else {
          phi->addIncoming(original_value, info.end_block);
        }
      }
      if (!has_else) {
        phi->addIncoming(original_value, last_cond_block);
      }
      scope_stack_.Set(var_name, static_cast<llvm::Value *>(phi));
    }
  }

  value_ = nullptr;
  return Status::OK();
}

}  // namespace jitfusion

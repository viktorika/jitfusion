/*
 * @Author: victorika
 * @Date: 2025-01-22 15:46:32
 * @Last Modified by: victorika
 * @Last Modified time: 2025-04-16 14:54:11
 */
#include "codegen.h"
#include "status.h"

namespace jitfusion {

Status CodeGen::Visit(NoOPNode& no_op_node) {
  const auto& names = no_op_node.GetNames();
  const auto& args = no_op_node.GetArgs();
  const bool isolated = no_op_node.IsIsolated();
  for (size_t i = 0; i < args.size(); ++i) {
    // When isolated=true, PushScope/PopScope is used to isolate variables between
    // independent child groups. Each child runs in its own scope so that variables
    // defined in one group are not visible to others. The scope is simply discarded
    // after PopScope — no modified variables are collected.
    // NOTE: This scope operation differs from the one in IfBlockNode, which uses
    // PushScope/PopScope for branch-level scoping and PHI node generation.
    // The two can safely nest without interference.
    if (isolated) {
      scope_stack_.PushScope();
    }
    llvm::Value* args_value;
    JF_RETURN_NOT_OK(GetValue(args[i].get(), &args_value));
    if (!names[i].empty()) {
      scope_stack_.Set(names[i], args_value);
    }
    if (isolated) {
      scope_stack_.PopScope();
    }
  }
  value_ = nullptr;
  return Status::OK();
}

}  // namespace jitfusion
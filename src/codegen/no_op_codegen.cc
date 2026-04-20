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
  for (size_t i = 0; i < args.size(); ++i) {
    llvm::Value* args_value;
    JF_RETURN_NOT_OK(GetValue(args[i].get(), &args_value));
    if (!names[i].empty()) {
      scope_stack_.Set(names[i], args_value);
    }
  }
  value_ = nullptr;
  return Status::OK();
}

}  // namespace jitfusion
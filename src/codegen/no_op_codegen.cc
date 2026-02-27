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
  for (const auto& arg : no_op_node.GetArgs()) {
    llvm::Value* args_value;
    JF_RETURN_NOT_OK(GetValue(arg.get(), &args_value));
  }
  value_ = nullptr;
  return Status::OK();
}

}  // namespace jitfusion
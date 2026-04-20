/*
 * @Author: victorika
 * @Date: 2026-04-14 11:21:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-14 11:21:00
 */
#include "codegen.h"
#include "status.h"

namespace jitfusion {

Status CodeGen::Visit(RefNode& ref_node) {
  llvm::Value* val = scope_stack_.Lookup(ref_node.GetName());
  if (nullptr == val) {
    return Status::RuntimeError("Variable not found: ", ref_node.GetName());
  }
  value_ = val;
  return Status::OK();
}

}  // namespace jitfusion

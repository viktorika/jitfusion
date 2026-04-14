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
  auto it = named_values_.find(ref_node.GetName());
  if (it == named_values_.end()) {
    return Status::RuntimeError("Variable not found: ", ref_node.GetName());
  }
  value_ = it->second;
  return Status::OK();
}

}  // namespace jitfusion

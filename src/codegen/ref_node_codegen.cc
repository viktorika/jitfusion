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
  auto val = scope_stack_.Lookup(ref_node.GetName());
  if (!val.has_value()) {
    return Status::RuntimeError("[internal] variable not found in codegen scope: ", ref_node.GetName(),
                                " (should be caught by validator)");
  }
  value_ = *val;
  return Status::OK();
}

}  // namespace jitfusion

/*
 * @Author: victorika
 * @Date: 2025-01-21 10:26:55
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-21 16:14:12
 */
#include "codegen.h"

namespace jitfusion {

Status CodeGen::Visit(ExecContextNode& /*exec_context_node*/) {
  value_ = ctx_.entry_function->getArg(1);
  return Status::OK();
}

}  // namespace jitfusion
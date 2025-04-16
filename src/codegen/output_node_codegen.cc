/*
 * @Author: victorika
 * @Date: 2025-04-16 11:18:42
 * @Last Modified by: victorika
 * @Last Modified time: 2025-04-16 11:19:06
 */
#include "codegen.h"
#include "exec_node.h"
#include "status.h"

namespace jitfusion {

Status CodeGen::Visit(OutputNode& /*output_node*/) {
  value_ = ctx_.entry_function->getArg(2);
  return Status::OK();
}

}  // namespace jitfusion
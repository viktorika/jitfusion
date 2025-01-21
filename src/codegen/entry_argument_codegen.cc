/*
 * @Author: victorika
 * @Date: 2025-01-20 15:08:52
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-20 16:08:09
 */
#include "codegen.h"
#include "status.h"

namespace jitfusion {

Status CodeGen::Visit(EntryArgumentNode& /*entry_argument_node*/) {
  value_ = ctx_.entry_function->getArg(0);
  return Status::OK();
}

}  // namespace jitfusion
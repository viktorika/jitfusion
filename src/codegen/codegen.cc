/*
 * @Author: victorika
 * @Date: 2025-01-20 14:47:10
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-21 10:26:15
 */
#include "codegen.h"
#include "exec_node.h"

namespace jitfusion {

Status CodeGen::GetValue(ExecNode *node, llvm::Value **ret_value) {
  RETURN_NOT_OK(node->Accept(this));
  *ret_value = value_;
  return Status::OK();
}

}  // namespace jitfusion
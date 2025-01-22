/*
 * @Author: victorika
 * @Date: 2025-01-22 15:46:32
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-22 15:49:40
 */
#include "codegen.h"
#include "status.h"

namespace jitfusion {

Status CodeGen::Visit(NoOPNode& /*no_op_node*/) {
  value_ = llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx_.context), 0, true);
  return Status::OK();
}

}  // namespace jitfusion
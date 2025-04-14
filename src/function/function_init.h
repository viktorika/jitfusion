/*
 * @Author: victorika
 * @Date: 2025-01-23 10:37:20
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-23 14:42:12
 */
#include "function_registry.h"
#include "status.h"

namespace jitfusion {

Status InitMathInternalFunc(FunctionRegistry *reg);
Status InitStringInternalFunc(FunctionRegistry *reg);
Status InitListInternalFunc(FunctionRegistry *reg);

inline void ReadOnlyFunctionAttributeSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/,
                                            llvm::Function *f) {
  f->addFnAttr(llvm::Attribute::ReadOnly);
  f->addFnAttr(llvm::Attribute::NoUnwind);
}

}  // namespace jitfusion
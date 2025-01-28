/*
 * @Author: victorika
 * @Date: 2025-01-15 10:48:46
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-24 11:00:05
 */
#pragma once

#include <type.h>
#include <memory>
#include "arena.h"
#include "exec_node.h"
#include "function_registry.h"
#include "status.h"

namespace jitfusion {

struct ExecContext {
  Arena arena;
};

class ExecEngine {
 public:
  ExecEngine() = default;
  ~ExecEngine() { delete engine_; }

  Status Compile(const std::unique_ptr<ExecNode>& exec_node, const std::unique_ptr<FunctionRegistry>& func_registry);
  Status Execute(void* entry_arguments, RetType* result);

 private:
  Arena const_value_arena_;
  llvm::LLVMContext llvm_context_;
  llvm::ExecutionEngine* engine_;
  uint64_t entry_func_ptr_;
  ValueType ret_type_;
};

}  // namespace jitfusion
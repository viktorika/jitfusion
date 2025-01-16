/*
 * @Author: victorika
 * @Date: 2025-01-15 10:48:46
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-16 16:33:48
 */
#pragma once

#include <memory>
#include "exec_node.h"
#include "function_registry.h"
#include "status.h"

namespace jitfusion {

class ExecEngine {
 public:
  Status Compile(const std::unique_ptr<ExecNode>& exec_node, const std::unique_ptr<FunctionRegistry>& func_registry);
  Status Execute(void* entry_arguments);
};

}  // namespace jitfusion
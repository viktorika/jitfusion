/*
 * @Author: victorika
 * @Date: 2025-01-16 16:30:31
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-20 15:03:41
 */
#pragma once

#include <memory>
#include "exec_node.h"
#include "function_registry.h"

namespace jitfusion {

class Validator : public Visitor {
 public:
  explicit Validator(const std::unique_ptr<FunctionRegistry>& func_registry) : func_registry_(func_registry) {}

  Status Validate(ExecNode* node);

  Status Visit(EntryArgumentNode& entry_argument_node) override;
  Status Visit(ExecContextNode& exec_context_node) override;
  Status Visit(ConstantValueNode& const_node) override;
  Status Visit(ConstantListValueNode& const_list_node) override;
  Status Visit(UnaryOPNode& unary_op_node) override;
  Status Visit(BinaryOPNode& binary_op_node) override;
  Status Visit(FunctionNode& function_node) override;

  const std::unique_ptr<FunctionRegistry>& func_registry_;
};

}  // namespace jitfusion
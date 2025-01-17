/*
 * @Author: victorika
 * @Date: 2025-01-16 16:30:31
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-17 14:35:58
 */
#pragma once

#include <memory>
#include "exec_node.h"
#include "function_registry.h"

namespace jitfusion {

class Validator : public Visitor {
 public:
  explicit Validator(const std::unique_ptr<FunctionRegistry>& func_registry) : func_registry_(func_registry) {}

  Status Validate(const std::unique_ptr<ExecNode>& node);

  Status Visit(EntryArgumentNode& entry_argument_node) override = 0;
  Status Visit(ExecContextNode& exec_context_node) override = 0;
  Status Visit(ConstantValueNode& const_node) override = 0;
  Status Visit(ConstantListValueNode& const_list_node) override = 0;
  Status Visit(UnaryOPNode& unary_op_node) override = 0;
  Status Visit(BinaryOPNode& binary_op_node) override = 0;
  Status Visit(FunctionNode& function_node) override = 0;

  const std::unique_ptr<FunctionRegistry>& func_registry_;
};

}  // namespace jitfusion
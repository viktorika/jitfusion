/*
 * @Author: victorika
 * @Date: 2025-01-16 16:30:31
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-23 15:48:55
 */
#pragma once

#include <memory>
#include "exec_node.h"
#include "function_registry.h"
#include "scope_stack.h"

namespace jitfusion {

struct VarEntry {
  ValueType type{ValueType::kUnknown};
  bool is_const{false};
};

enum class SetVarResult : uint8_t { kOk, kConstReassign, kRedeclare, kTypeMismatch };

class Validator : public Visitor {
 public:
  explicit Validator(const std::unique_ptr<FunctionRegistry>& func_registry) : func_registry_(func_registry) {}

  Status Validate(ExecNode* node);

  Status Visit(EntryArgumentNode& entry_argument_node) override;
  Status Visit(ExecContextNode& exec_context_node) override;
  Status Visit(OutputNode& output_node) override;
  Status Visit(ConstantValueNode& const_node) override;
  Status Visit(ConstantListValueNode& const_list_node) override;
  Status Visit(UnaryOPNode& unary_op_node) override;
  Status Visit(BinaryOPNode& binary_op_node) override;
  Status Visit(FunctionNode& function_node) override;
  Status Visit(NoOPNode& no_op_node) override;
  Status Visit(IfNode& if_node) override;
  Status Visit(SwitchNode& switch_node) override;
  Status Visit(IfBlockNode& if_block_node) override;
  Status Visit(RefNode& ref_node) override;

 private:
  SetVarResult TrySetVar(const std::string& name, ValueType type, bool is_const);

  const std::unique_ptr<FunctionRegistry>& func_registry_;
  ScopeStack<VarEntry> type_scope_stack_;
};

}  // namespace jitfusion
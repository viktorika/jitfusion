/*
 * @Author: victorika
 * @Date: 2025-01-20 14:16:13
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-23 16:00:08
 */
#pragma once

#include "exec_node.h"
#include "function_registry.h"
#include "type.h"

namespace jitfusion {

class CodeGen : public Visitor {
 public:
  explicit CodeGen(IRCodeGenContext &ctx) : ctx_(ctx) {}

  Status GetValue(ExecNode *node, llvm::Value **ret_value);

  Status Visit(EntryArgumentNode &entry_argument_node) override;
  Status Visit(ExecContextNode &exec_context_node) override;
  Status Visit(ConstantValueNode &const_node) override;
  Status Visit(ConstantListValueNode &const_list_node) override;
  Status Visit(UnaryOPNode &unary_op_node) override;
  Status Visit(BinaryOPNode &binary_op_node) override;
  Status Visit(FunctionNode &function_node) override;
  Status Visit(NoOPNode &no_op_node) override;
  Status Visit(IfNode& if_node) override;
  Status Visit(SwitchNode& switch_node) override;

  Status NumericTypeConvert(ValueType from, ValueType to, llvm::Value **value);
  Status ToBoolean(ValueType from, llvm::Value **value);

 private:
  Status SolveBinaryOpNumericType(BinaryOPNode &binary_node, llvm::Value *lhs_value, llvm::Value *rhs_value);
  Status SolveBinaryOpComplexType(BinaryOPNode &binary_node, llvm::Value *lhs_value, llvm::Value *rhs_value);
  Status ValueTypeToLLVMType(ValueType value_type, llvm::Type **llvm_type);

  IRCodeGenContext &ctx_;
  llvm::Value *value_{nullptr};
};

}  // namespace jitfusion
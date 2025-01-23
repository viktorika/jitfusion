/*
 * @Author: victorika
 * @Date: 2025-01-15 10:59:27
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-23 14:57:59
 */
#include "exec_node.h"
#include "type.h"

namespace jitfusion {

std::string ExecNode::ToString(const std::string& prefix) { return ToStringImpl(prefix); }

std::string EntryArgumentNode::ToStringImpl(const std::string& prefix) { return prefix + "|--entry_argument\n"; }
Status EntryArgumentNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType EntryArgumentNode::GetExecNodeType() { return ExecNodeType::kEntryArgumentNode; }

std::string ExecContextNode::ToStringImpl(const std::string& prefix) { return prefix + "|--exec_context\n"; }
Status ExecContextNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType ExecContextNode::GetExecNodeType() { return ExecNodeType::kExecContextNode; }

struct ValueToString {
  template <typename T>
  std::string operator()(const T& value) const {
    if constexpr (std::is_same_v<std::string, T>) {
      return value;
    } else {
      return std::to_string(value);
    }
  }
};

std::string ConstantValueNode::ToStringImpl(const std::string& prefix) {
  return prefix + "|--" + std::visit(ValueToString{}, val_) + "\n";
}

Status ConstantValueNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType ConstantValueNode::GetExecNodeType() { return ExecNodeType::kConstValueNode; }

struct VectorValueToString {
  template <typename T>
  std::string operator()(const T& value_list) const {
    std::string result = "[";
    bool is_first = true;
    for (const auto& value : value_list) {
      if (!is_first) {
        result += ",";
      }
      if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        result += value;
      } else {
        result += std::to_string(value);
      }
      is_first = false;
    }
    result += "]";
    return result;
  }
};

std::string ConstantListValueNode::ToStringImpl(const std::string& prefix) {
  return prefix + "|--" + std::visit(VectorValueToString{}, val_list_) + "\n";
}

Status ConstantListValueNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType ConstantListValueNode::GetExecNodeType() { return ExecNodeType::kConstListValueNode; }

std::string UnaryOPNode::ToStringImpl(const std::string& prefix) {
  std::string result = prefix + "|--" + TypeHelper::UnaryOPTypeToString(op_) + "\n" + child_->ToString(prefix + "|   ");
  return result;
}

Status UnaryOPNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType UnaryOPNode::GetExecNodeType() { return ExecNodeType::kUnaryOPType; }

std::string BinaryOPNode::ToStringImpl(const std::string& prefix) {
  return prefix + "|--" + TypeHelper::BinaryOPTypeToString(op_) + "\n" + left_->ToString(prefix + "|   ") +
         right_->ToString(prefix + "|   ");
}

Status BinaryOPNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType BinaryOPNode::GetExecNodeType() { return ExecNodeType::kBinaryOPNode; }

std::string FunctionNode::ToStringImpl(const std::string& prefix) {
  std::string result = prefix + "|--" + func_name_ + "\n";
  for (const auto& child : args_) {
    result += child->ToString(prefix + "|   ");
  }
  return result;
}

void FunctionNode::AppendArgs(std::unique_ptr<ExecNode>&& arg) { args_.emplace_back(std::move(arg)); }
Status FunctionNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType FunctionNode::GetExecNodeType() { return ExecNodeType::kFunctionNode; }

std::string NoOPNode::ToStringImpl(const std::string& prefix) { return prefix + "|--no_op\n"; }

Status NoOPNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType NoOPNode::GetExecNodeType() { return ExecNodeType::kNoOPNode; }
void NoOPNode::AppendArgs(std::unique_ptr<ExecNode>&& arg) { args_.emplace_back(std::move(arg)); }

std::string IfNode::ToStringImpl(const std::string& prefix) {
  std::string result = prefix + "|--if\n";
  for (const auto& child : args_) {
    result += child->ToString(prefix + "|   ");
  }
  return result;
}

Status IfNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType IfNode::GetExecNodeType() { return ExecNodeType::kIfNode; }
void IfNode::AppendArgs(std::unique_ptr<ExecNode>&& arg) { args_.emplace_back(std::move(arg)); }

}  // namespace jitfusion
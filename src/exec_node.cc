/*
 * @Author: victorika
 * @Date: 2025-01-15 10:59:27
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-23 15:47:55
 */
#include "exec_node.h"
#include "type.h"

namespace jitfusion {

std::string ExecNode::ToString(const std::string& prefix) { return ToStringImpl(prefix); }

std::string EntryArgumentNode::ToStringImpl(const std::string& prefix) { return prefix + "|--entry_argument\n"; }
Status EntryArgumentNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType EntryArgumentNode::GetExecNodeType() { return ExecNodeType::kEntryArgumentNode; }
std::unique_ptr<ExecNode> EntryArgumentNode::Clone() { return std::make_unique<EntryArgumentNode>(); }

std::string ExecContextNode::ToStringImpl(const std::string& prefix) { return prefix + "|--exec_context\n"; }
Status ExecContextNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType ExecContextNode::GetExecNodeType() { return ExecNodeType::kExecContextNode; }
std::unique_ptr<ExecNode> ExecContextNode::Clone() { return std::make_unique<ExecContextNode>(); }

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
std::unique_ptr<ExecNode> ConstantValueNode::Clone() { return std::make_unique<ConstantValueNode>(*this); }

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
std::unique_ptr<ExecNode> ConstantListValueNode::Clone() { return std::make_unique<ConstantListValueNode>(*this); }

std::string UnaryOPNode::ToStringImpl(const std::string& prefix) {
  std::string result = prefix + "|--" + TypeHelper::UnaryOPTypeToString(op_) + "\n" + child_->ToString(prefix + "|   ");
  return result;
}

Status UnaryOPNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType UnaryOPNode::GetExecNodeType() { return ExecNodeType::kUnaryOPNode; }
std::unique_ptr<ExecNode> UnaryOPNode::Clone() { return std::make_unique<UnaryOPNode>(op_, child_->Clone()); }

std::string BinaryOPNode::ToStringImpl(const std::string& prefix) {
  return prefix + "|--" + TypeHelper::BinaryOPTypeToString(op_) + "\n" + left_->ToString(prefix + "|   ") +
         right_->ToString(prefix + "|   ");
}

Status BinaryOPNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType BinaryOPNode::GetExecNodeType() { return ExecNodeType::kBinaryOPNode; }

std::unique_ptr<ExecNode> BinaryOPNode::Clone() {
  return std::make_unique<BinaryOPNode>(op_, left_->Clone(), right_->Clone());
}

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

std::unique_ptr<ExecNode> FunctionNode::Clone() {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.reserve(args_.size());
  for (const auto& arg : args_) {
    args.emplace_back(arg->Clone());
  }
  return std::make_unique<FunctionNode>(func_name_, std::move(args));
}

std::string NoOPNode::ToStringImpl(const std::string& prefix) { return prefix + "|--no_op\n"; }

Status NoOPNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType NoOPNode::GetExecNodeType() { return ExecNodeType::kNoOPNode; }
void NoOPNode::AppendArgs(std::unique_ptr<ExecNode>&& arg) { args_.emplace_back(std::move(arg)); }

std::unique_ptr<ExecNode> NoOPNode::Clone() {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.reserve(args_.size());
  for (const auto& arg : args_) {
    args.emplace_back(arg->Clone());
  }
  return std::make_unique<NoOPNode>(std::move(args));
}

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

std::unique_ptr<ExecNode> IfNode::Clone() {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.reserve(args_.size());
  for (const auto& arg : args_) {
    args.emplace_back(arg->Clone());
  }
  return std::make_unique<IfNode>(std::move(args));
}

std::string SwitchNode::ToStringImpl(const std::string& prefix) {
  std::string result = prefix + "|--switch\n";
  for (const auto& child : args_) {
    result += child->ToString(prefix + "|   ");
  }
  return result;
}

Status SwitchNode::Accept(Visitor* visitor) { return visitor->Visit(*this); }
ExecNodeType SwitchNode::GetExecNodeType() { return ExecNodeType::kSwitchNode; }
void SwitchNode::AppendArgs(std::unique_ptr<ExecNode>&& arg) { args_.emplace_back(std::move(arg)); }

std::unique_ptr<ExecNode> SwitchNode::Clone() {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.reserve(args_.size());
  for (const auto& arg : args_) {
    args.emplace_back(arg->Clone());
  }
  return std::make_unique<SwitchNode>(std::move(args));
}

}  // namespace jitfusion
#include "exec_node.h"

namespace jitfusion {

std::string ExecNode::ToString(const std::string& prefix) { return ToStringImpl(prefix); }

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

Status ConstantValueNode::Accept(Visitor* visitor) { return visitor->visit(*this); }

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

Status ConstantListValueNode::Accept(Visitor* visitor) { return visitor->visit(*this); }

inline std::string UnaryOPNode::ToStringImpl(const std::string& prefix) {
  std::string result = prefix + "|--" + UnaryOPTypeToString(op_) + "\n" + child_->ToString(prefix + "|   ");
  return result;
}

Status UnaryOPNode::Accept(Visitor* visitor) { return visitor->visit(*this); }

std::string BinaryOPNode::ToStringImpl(const std::string& prefix) {
  return prefix + "|--" + BinaryOPTypeToString(op_) + "\n" + left_->ToString(prefix + "|   ") +
         right_->ToString(prefix + "|   ");
}

Status BinaryOPNode::Accept(Visitor* visitor) { return visitor->visit(*this); }

}  // namespace jitfusion
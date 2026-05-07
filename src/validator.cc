/*
 * @Author: victorika
 * @Date: 2025-01-16 16:35:04
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-30 22:22:27
 */
#include "validator.h"
#include <exec_node.h>
#include "diagnostic.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {

// Build a Status::ParseError whose message is a rendered Diagnostic. The
// Diagnostic pulls both its source location and the original source text
// from `node.GetLocation()`, so no separate source_code argument is needed.
template <typename... Notes>
Status MakeParseError(const ExecNode& node, std::string message, Notes&&... notes) {
  Diagnostic diag(Diagnostic::Level::kError, std::move(message), node.GetLocation());
  (diag.WithNote(std::forward<Notes>(notes)), ...);
  return Status::ParseError(diag.Render());
}

}  // namespace

Status Validator::Validate(ExecNode* node) { return node->Accept(this); }

Status Validator::Visit(EntryArgumentNode& entry_argument_node) {
  entry_argument_node.SetReturnType(ValueType::kPtr);
  return Status::OK();
}

Status Validator::Visit(ExecContextNode& exec_context_node) {
  exec_context_node.SetReturnType(ValueType::kPtr);
  return Status::OK();
}

Status Validator::Visit(OutputNode& output_node) {
  output_node.SetReturnType(ValueType::kPtr);
  return Status::OK();
}

struct ConstantValueVisitor {
  ValueType operator()(int8_t /*v*/) { return ValueType::kI8; }
  ValueType operator()(int16_t /*v*/) { return ValueType::kI16; }
  ValueType operator()(int32_t /*v*/) { return ValueType::kI32; }
  ValueType operator()(int64_t /*v*/) { return ValueType::kI64; }
  ValueType operator()(uint8_t /*v*/) { return ValueType::kU8; }
  ValueType operator()(uint16_t /*v*/) { return ValueType::kU16; }
  ValueType operator()(uint32_t /*v*/) { return ValueType::kU32; }
  ValueType operator()(uint64_t /*v*/) { return ValueType::kU64; }
  ValueType operator()(float /*v*/) { return ValueType::kF32; }
  ValueType operator()(double /*v*/) { return ValueType::kF64; }
  ValueType operator()(const std::string& /*v*/) { return ValueType::kString; }
};

Status Validator::Visit(ConstantValueNode& const_node) {
  const_node.SetReturnType(std::visit(ConstantValueVisitor(), const_node.GetVal()));
  return Status::OK();
}

struct ConstantListValueVisitor {
  ValueType operator()(const std::vector<int8_t>& /*v*/) { return ValueType::kI8List; }
  ValueType operator()(const std::vector<int16_t>& /*v*/) { return ValueType::kI16List; }
  ValueType operator()(const std::vector<int32_t>& /*v*/) { return ValueType::kI32List; }
  ValueType operator()(const std::vector<int64_t>& /*v*/) { return ValueType::kI64List; }
  ValueType operator()(const std::vector<uint8_t>& /*v*/) { return ValueType::kU8List; }
  ValueType operator()(const std::vector<uint16_t>& /*v*/) { return ValueType::kU16List; }
  ValueType operator()(const std::vector<uint32_t>& /*v*/) { return ValueType::kU32List; }
  ValueType operator()(const std::vector<uint64_t>& /*v*/) { return ValueType::kU64List; }
  ValueType operator()(const std::vector<float>& /*v*/) { return ValueType::kF32List; }
  ValueType operator()(const std::vector<double>& /*v*/) { return ValueType::kF64List; }
  ValueType operator()(const std::vector<std::string>& /*v*/) { return ValueType::kStringList; }
};

Status Validator::Visit(ConstantListValueNode& list_node) {
  list_node.SetReturnType(std::visit(ConstantListValueVisitor(), list_node.GetValList()));
  return Status::OK();
}

Status Validator::Visit(UnaryOPNode& unary_op_node) {
  JF_RETURN_NOT_OK(unary_op_node.GetChild()->Accept(this));
  auto child_return_type = unary_op_node.GetChild()->GetReturnType();
  if (ValueType::kVoid == child_return_type) {
    return MakeParseError(unary_op_node, "unary operator does not support void operand");
  }
  if (!TypeHelper::IsNumericType(child_return_type)) {
    return MakeParseError(
        unary_op_node, "unary operator requires a numeric operand, got " + TypeHelper::TypeToString(child_return_type));
  }

  switch (unary_op_node.GetOp()) {
    case UnaryOPType::kPlus:
      unary_op_node.SetReturnType(child_return_type);
      break;
    case UnaryOPType::kMinus: {
      switch (child_return_type) {
        case ValueType::kI8:
        case ValueType::kU8:
        case ValueType::kI16:
        case ValueType::kU16:
          unary_op_node.SetReturnType(ValueType::kI32);
          break;
        case ValueType::kI32:
        case ValueType::kU32:
        case ValueType::kI64:
        case ValueType::kU64:
        case ValueType::kF32:
        case ValueType::kF64:
          unary_op_node.SetReturnType(child_return_type);
          break;
        default:
          return Status::NotImplemented("[internal] unary minus not implemented for type ",
                                        TypeHelper::TypeToString(child_return_type),
                                        " (should have been caught by the numeric-type check above)");
      }
    } break;
    case UnaryOPType::kNot:
      unary_op_node.SetReturnType(ValueType::kU8);
      break;
    case UnaryOPType::kBitwiseNot: {
      if (!TypeHelper::IsIntegerType(child_return_type)) {
        return MakeParseError(unary_op_node, "bitwise-not requires an integer operand, got " +
                                                 TypeHelper::TypeToString(child_return_type));
      }
      unary_op_node.SetReturnType(child_return_type);
    } break;
    default:
      return Status::NotImplemented("[internal] UnaryOPType ", TypeHelper::UnaryOPTypeToString(unary_op_node.GetOp()),
                                    " not implemented (compiler bug)");
  }
  return Status::OK();
}

namespace {

struct DivZeroVisit {
  bool operator()(int8_t v) { return 0 == v; }
  bool operator()(int16_t v) { return 0 == v; }
  bool operator()(int32_t v) { return 0 == v; }
  bool operator()(int64_t v) { return 0 == v; }
  bool operator()(uint8_t v) { return 0 == v; }
  bool operator()(uint16_t v) { return 0 == v; }
  bool operator()(uint32_t v) { return 0 == v; }
  bool operator()(uint64_t v) { return 0 == v; }
  bool operator()(float /*v*/) { return false; }
  bool operator()(double /*v*/) { return false; }
  bool operator()(const std::string& /*v*/) { return false; }
};

}  // namespace

Status Validator::Visit(BinaryOPNode& binary_op_node) {
  auto* left = binary_op_node.GetLeft();
  auto* right = binary_op_node.GetRight();
  JF_RETURN_NOT_OK(left->Accept(this));
  JF_RETURN_NOT_OK(right->Accept(this));
  auto op = binary_op_node.GetOp();
  auto lhs_type = left->GetReturnType();
  auto rhs_type = right->GetReturnType();

  if (ValueType::kVoid == lhs_type || ValueType::kVoid == rhs_type) {
    return MakeParseError(binary_op_node, "binary operator does not support void operand");
  }

  // 1) Logical operators (and, or): both sides must be numeric.
  if (TypeHelper::IsLogicalBinaryOPType(op)) {
    if (!TypeHelper::IsNumericType(lhs_type) || !TypeHelper::IsNumericType(rhs_type)) {
      return MakeParseError(binary_op_node, "logical operator requires numeric operands, got " +
                                                TypeHelper::TypeToString(lhs_type) + " and " +
                                                TypeHelper::TypeToString(rhs_type) + " for '" +
                                                TypeHelper::BinaryOPTypeToString(op) + "'");
    }
    binary_op_node.SetReturnType(ValueType::kU8);
    return Status::OK();
  }

  // 2) Relational operators (<, <=, ==, !=, >, >=): either both numeric or both string.
  if (TypeHelper::IsRelationalBinaryOPType(op)) {
    const bool both_numeric = TypeHelper::IsNumericType(lhs_type) && TypeHelper::IsNumericType(rhs_type);
    const bool both_string = (lhs_type == ValueType::kString) && (rhs_type == ValueType::kString);
    if (!both_numeric && !both_string) {
      return MakeParseError(binary_op_node,
                            "relational operator requires both operands to be numeric or both to be string, got " +
                                TypeHelper::TypeToString(lhs_type) + " and " + TypeHelper::TypeToString(rhs_type) +
                                " for '" + TypeHelper::BinaryOPTypeToString(op) + "'");
    }
    binary_op_node.SetReturnType(ValueType::kU8);
    return Status::OK();
  }

  // 3) Bitwise operators (&, |, ^, <<, >>): both sides must be integer.
  if (op == BinaryOPType::kBitwiseAnd || op == BinaryOPType::kBitwiseOr || op == BinaryOPType::kBitwiseXor ||
      op == BinaryOPType::kBitwiseShiftLeft || op == BinaryOPType::kBitwiseShiftRight) {
    if (!TypeHelper::IsIntegerType(lhs_type) || !TypeHelper::IsIntegerType(rhs_type)) {
      return MakeParseError(binary_op_node, "bitwise operator requires integer operands, got " +
                                                TypeHelper::TypeToString(lhs_type) + " and " +
                                                TypeHelper::TypeToString(rhs_type) + " for '" +
                                                TypeHelper::BinaryOPTypeToString(op) + "'");
    }
    if (op == BinaryOPType::kBitwiseShiftLeft || op == BinaryOPType::kBitwiseShiftRight) {
      binary_op_node.SetReturnType(lhs_type);
    } else {
      binary_op_node.SetReturnType(TypeHelper::GetPromotedType(lhs_type, rhs_type));
    }
    return Status::OK();
  }

  // 4) Arithmetic operators.
  if (op == BinaryOPType::kAdd) {
    // kAdd allows numeric+numeric, string+string (concat) or same-list+same-list (concat).
    if (TypeHelper::IsNumericType(lhs_type) && TypeHelper::IsNumericType(rhs_type)) {
      binary_op_node.SetReturnType(TypeHelper::GetPromotedType(lhs_type, rhs_type));
      return Status::OK();
    }
    if (lhs_type == ValueType::kString && rhs_type == ValueType::kString) {
      binary_op_node.SetReturnType(ValueType::kString);
      return Status::OK();
    }
    if (TypeHelper::IsListType(lhs_type) && lhs_type == rhs_type) {
      binary_op_node.SetReturnType(lhs_type);
      return Status::OK();
    }
    return MakeParseError(binary_op_node,
                          "operator '+' requires both operands to be numeric, both string, or two lists of the "
                          "same type, got " +
                              TypeHelper::TypeToString(lhs_type) + " and " + TypeHelper::TypeToString(rhs_type));
  }

  // kSub/kMul/kDiv/kMod: both sides must be numeric.
  if (!TypeHelper::IsNumericType(lhs_type) || !TypeHelper::IsNumericType(rhs_type)) {
    return MakeParseError(binary_op_node, "arithmetic operator requires numeric operands, got " +
                                              TypeHelper::TypeToString(lhs_type) + " and " +
                                              TypeHelper::TypeToString(rhs_type) + " for '" +
                                              TypeHelper::BinaryOPTypeToString(op) + "'");
  }
  if ((op == BinaryOPType::kDiv || op == BinaryOPType::kMod) &&
      right->GetExecNodeType() == ExecNodeType::kConstValueNode &&
      std::visit(DivZeroVisit(), static_cast<ConstantValueNode*>(right)->GetVal())) {
    return MakeParseError(binary_op_node, "division or modulo by constant zero");
  }
  binary_op_node.SetReturnType(TypeHelper::GetPromotedType(lhs_type, rhs_type));
  return Status::OK();
}

Status Validator::Visit(FunctionNode& function_node) {
  std::vector<ValueType> arg_types;
  arg_types.reserve(function_node.GetArgs().size());
  for (const auto& arg : function_node.GetArgs()) {
    JF_RETURN_NOT_OK(arg->Accept(this));
    if (ValueType::kVoid == arg->GetReturnType()) {
      return MakeParseError(function_node, "function '" + function_node.GetFuncName() + "' argument cannot be void");
    }
    arg_types.emplace_back(arg->GetReturnType());
  }
  FunctionSignature sign(function_node.GetFuncName(), arg_types, ValueType::kUnknown);
  FunctionStructure function_structure;
  if (auto st = func_registry_->GetFuncBySign(sign, &function_structure); !st.ok()) {
    // Upgrade the registry's message to a diagnostic carrying source location.
    return MakeParseError(function_node, st.ToString());
  }
  function_node.SetReturnType(sign.GetRetType());
  return Status::OK();
}

Status Validator::Visit(NoOPNode& no_op_node) {
  const auto& names = no_op_node.GetNames();
  const auto& args = no_op_node.GetArgs();
  const bool isolated = no_op_node.IsIsolated();
  for (size_t i = 0; i < args.size(); ++i) {
    // When isolated=true, PushScope/PopScope is used to isolate variable types
    // between independent child groups. Each child runs in its own type scope so
    // that variable types defined in one group are not visible to others. The scope
    // is simply discarded after PopScope — no modified types are collected.
    // NOTE: This scope operation differs from the one in IfBlockNode, which uses
    // PushScope/PopScope for branch-level type scoping and type consistency checks.
    // The two can safely nest without interference.
    if (isolated) {
      type_scope_stack_.PushScope();
    }
    JF_RETURN_NOT_OK(args[i]->Accept(this));
    if (!names[i].empty()) {
      type_scope_stack_.Set(names[i], args[i]->GetReturnType());
    }
    if (isolated) {
      type_scope_stack_.PopScope();
    }
  }
  no_op_node.SetReturnType(ValueType::kVoid);
  return Status::OK();
}

Status Validator::Visit(IfNode& if_node) {
  if (if_node.GetArgs().size() != 3) {
    return MakeParseError(if_node, "if expects exactly 3 arguments (condition, then, else), got " +
                                       std::to_string(if_node.GetArgs().size()));
  }
  std::vector<ValueType> arg_types;
  arg_types.reserve(if_node.GetArgs().size());
  for (const auto& arg : if_node.GetArgs()) {
    JF_RETURN_NOT_OK(arg->Accept(this));
    if (ValueType::kVoid == arg->GetReturnType()) {
      return MakeParseError(*arg, "if argument cannot be void");
    }
    arg_types.emplace_back(arg->GetReturnType());
  }

  if (!TypeHelper::IsNumericType(arg_types[0])) {
    return MakeParseError(*if_node.GetArgs()[0],
                          "if condition must be a numeric type, got " + TypeHelper::TypeToString(arg_types[0]));
  }

  if (TypeHelper::IsNumericType(arg_types[1]) && TypeHelper::IsNumericType(arg_types[2])) {
    if_node.SetReturnType(TypeHelper::GetPromotedType(arg_types[1], arg_types[2]));
  } else if (arg_types[1] == arg_types[2]) {
    if_node.SetReturnType(arg_types[1]);
  } else {
    return MakeParseError(if_node, "if branches have incompatible types: " + TypeHelper::TypeToString(arg_types[1]) +
                                       " vs " + TypeHelper::TypeToString(arg_types[2]));
  }
  return Status::OK();
}

Status Validator::Visit(SwitchNode& switch_node) {
  auto size = switch_node.GetArgs().size();
  if (0U == (size & 1)) {
    return MakeParseError(switch_node,
                          "switch expects an odd number of arguments (cond, then, [cond, then, ...], default), got " +
                              std::to_string(size));
  }
  std::vector<ValueType> arg_types;
  arg_types.reserve(size);
  for (const auto& arg : switch_node.GetArgs()) {
    JF_RETURN_NOT_OK(arg->Accept(this));
    if (ValueType::kVoid == arg->GetReturnType()) {
      return MakeParseError(*arg, "switch argument cannot be void");
    }
    arg_types.emplace_back(arg->GetReturnType());
  }

  ValueType numeric_type = ValueType::kUnknown;
  ValueType complex_type = ValueType::kUnknown;
  for (std::size_t i = 0; i < size; i++) {
    if (i + 1 != size && i % 2 == 0 && !TypeHelper::IsNumericType(switch_node.GetArgs()[i]->GetReturnType())) {
      return MakeParseError(*switch_node.GetArgs()[i],
                            "switch condition must be a numeric type, got " +
                                TypeHelper::TypeToString(switch_node.GetArgs()[i]->GetReturnType()));
    }
    if (i % 2 != 0 || i + 1 == size) {
      if (TypeHelper::IsNumericType(switch_node.GetArgs()[i]->GetReturnType())) {
        numeric_type = TypeHelper::GetPromotedType(numeric_type, switch_node.GetArgs()[i]->GetReturnType());
      } else {
        if (complex_type != ValueType::kUnknown && complex_type != switch_node.GetArgs()[i]->GetReturnType()) {
          return MakeParseError(*switch_node.GetArgs()[i],
                                "switch branch has incompatible type: got " +
                                    TypeHelper::TypeToString(switch_node.GetArgs()[i]->GetReturnType()) +
                                    ", previously seen " + TypeHelper::TypeToString(complex_type));
        }
        complex_type = switch_node.GetArgs()[i]->GetReturnType();
      }
    }
  }

  if (complex_type != ValueType::kUnknown && numeric_type != ValueType::kUnknown) {
    return MakeParseError(switch_node, "switch branches have incompatible types: mix of numeric (" +
                                           TypeHelper::TypeToString(numeric_type) + ") and non-numeric (" +
                                           TypeHelper::TypeToString(complex_type) + ")");
  }
  switch_node.SetReturnType(numeric_type == ValueType::kUnknown ? complex_type : numeric_type);

  return Status::OK();
}

Status Validator::Visit(IfBlockNode& if_block_node) {
  const auto& args = if_block_node.GetArgs();
  int num_args = static_cast<int>(args.size());
  if (num_args < 2) {
    return MakeParseError(if_block_node, "if block must have at least a condition and a body");
  }
  bool has_else = (num_args % 2 != 0);
  int num_branches = num_args / 2;

  std::unordered_map<std::string, ValueType> all_shadowed;

  for (int i = 0; i < num_branches; ++i) {
    int cond_idx = i * 2;
    int body_idx = (i * 2) + 1;

    JF_RETURN_NOT_OK(args[cond_idx]->Accept(this));
    if (!TypeHelper::IsNumericType(args[cond_idx]->GetReturnType())) {
      return MakeParseError(*args[cond_idx], "if block condition must be a numeric type, got " +
                                                 TypeHelper::TypeToString(args[cond_idx]->GetReturnType()));
    }
    // Branch scope isolation: PushScope creates a new scope so that type modifications
    // within this branch are recorded separately. After executing the branch body,
    // GetShadowed() collects the shadowed variables for type consistency checks.
    // PopScope then discards the branch-local modifications.
    // NOTE: This scope operation differs from NoOPNode's isolated scope, which is
    // used for type isolation between independent child groups. The two can safely
    // nest without interference.
    type_scope_stack_.PushScope();
    JF_RETURN_NOT_OK(args[body_idx]->Accept(this));
    if (args[body_idx]->GetReturnType() != ValueType::kVoid) {
      return MakeParseError(*args[body_idx], "if block body must be void, got " +
                                                 TypeHelper::TypeToString(args[body_idx]->GetReturnType()));
    }
    auto shadowed = type_scope_stack_.GetShadowed();
    type_scope_stack_.PopScope();
    for (const auto& [name, new_type] : shadowed) {
      ValueType original_type = type_scope_stack_.Lookup(name);
      if (original_type != ValueType::kUnknown && original_type != new_type) {
        return MakeParseError(
            if_block_node,
            "variable '" + name + "' has incompatible types across if block branches: original type is " +
                TypeHelper::TypeToString(original_type) + ", but assigned " + TypeHelper::TypeToString(new_type));
      }
      all_shadowed[name] = new_type;
    }
  }

  if (has_else) {
    // Else branch scope isolation: same as the condition branches above —
    // PushScope, execute, GetShadowed, then PopScope.
    type_scope_stack_.PushScope();
    JF_RETURN_NOT_OK(args[num_args - 1]->Accept(this));
    if (args[num_args - 1]->GetReturnType() != ValueType::kVoid) {
      return MakeParseError(*args[num_args - 1], "if block else body must be void, got " +
                                                     TypeHelper::TypeToString(args[num_args - 1]->GetReturnType()));
    }
    auto shadowed = type_scope_stack_.GetShadowed();
    type_scope_stack_.PopScope();
    for (const auto& [name, new_type] : shadowed) {
      ValueType original_type = type_scope_stack_.Lookup(name);
      if (original_type != ValueType::kUnknown && original_type != new_type) {
        return MakeParseError(
            if_block_node,
            "variable '" + name + "' has incompatible types across if block else branch: original type is " +
                TypeHelper::TypeToString(original_type) + ", but assigned " + TypeHelper::TypeToString(new_type));
      }
      all_shadowed[name] = new_type;
    }
  }

  for (const auto& [name, type] : all_shadowed) {
    type_scope_stack_.Set(name, type);
  }

  if_block_node.SetReturnType(ValueType::kVoid);
  return Status::OK();
}

Status Validator::Visit(RefNode& ref_node) {
  ValueType type = type_scope_stack_.Lookup(ref_node.GetName());
  if (type == ValueType::kUnknown) {
    return MakeParseError(ref_node, "variable '" + ref_node.GetName() + "' is not defined");
  }
  ref_node.SetReturnType(type);
  return Status::OK();
}

}  // namespace jitfusion
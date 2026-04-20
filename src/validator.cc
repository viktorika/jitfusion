/*
 * @Author: victorika
 * @Date: 2025-01-16 16:35:04
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-30 22:22:27
 */
#include "validator.h"
#include <exec_node.h>
#include "status.h"
#include "type.h"

namespace jitfusion {

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
    return Status::ParseError("Unary OP not support void type");
  }
  if (!TypeHelper::IsNumericType(child_return_type)) {
    return Status::ParseError("Unary OP only support numeric type");
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
          return Status::NotImplemented("Type ", TypeHelper::TypeToString(child_return_type),
                                        "  not implement minus op");
      }
    } break;
    case UnaryOPType::kNot:
      unary_op_node.SetReturnType(ValueType::kU8);
      break;
    case UnaryOPType::kBitwiseNot: {
      if (!TypeHelper::IsIntegerType(child_return_type)) {
        return Status::ParseError("BitwiseNot operation, child type is not integer");
      }
      unary_op_node.SetReturnType(child_return_type);
    } break;
    default:
      return Status::NotImplemented("UnaryOPType ", TypeHelper::UnaryOPTypeToString(unary_op_node.GetOp()),
                                    " not implemented");
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

  if (ValueType::kVoid == left->GetReturnType() || ValueType::kVoid == right->GetReturnType()) {
    return Status::ParseError("Binary OP not support void type");
  }

  // extra check
  if ((BinaryOPType::kDiv == op || BinaryOPType::kMod == op) &&
      right->GetExecNodeType() == ExecNodeType::kConstValueNode &&
      std::visit(DivZeroVisit(), static_cast<ConstantValueNode*>(right)->GetVal())) {
    return Status::ParseError("Cant no div/mod zero");
  }

  if (TypeHelper::IsLogicalBinaryOPType(binary_op_node.GetOp())) {
    if (!TypeHelper::IsNumericType(left->GetReturnType()) || !TypeHelper::IsNumericType(right->GetReturnType())) {
      return Status::ParseError("Logical operator only supports numeric types, got ",
                                TypeHelper::TypeToString(left->GetReturnType()), " and ",
                                TypeHelper::TypeToString(right->GetReturnType()));
    }
    binary_op_node.SetReturnType(ValueType::kU8);
    return Status::OK();
  }

  if (TypeHelper::IsRelationalBinaryOPType(binary_op_node.GetOp())) {
    binary_op_node.SetReturnType(ValueType::kU8);
    return Status::OK();
  }
  binary_op_node.SetReturnType(TypeHelper::GetPromotedType(left->GetReturnType(), right->GetReturnType()));
  return Status::OK();
}

Status Validator::Visit(FunctionNode& function_node) {
  std::vector<ValueType> arg_types;
  arg_types.reserve(function_node.GetArgs().size());
  for (const auto& arg : function_node.GetArgs()) {
    JF_RETURN_NOT_OK(arg->Accept(this));
    if (ValueType::kVoid == arg->GetReturnType()) {
      return Status::ParseError("Function ", function_node.GetFuncName(), " argument cant be void type");
    }
    arg_types.emplace_back(arg->GetReturnType());
  }
  FunctionSignature sign(function_node.GetFuncName(), arg_types, ValueType::kUnknown);
  FunctionStructure function_structure;
  JF_RETURN_NOT_OK(func_registry_->GetFuncBySign(sign, &function_structure));
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
    return Status::ParseError("If node must has 3 arguments");
  }
  std::vector<ValueType> arg_types;
  arg_types.reserve(if_node.GetArgs().size());
  for (const auto& arg : if_node.GetArgs()) {
    JF_RETURN_NOT_OK(arg->Accept(this));
    if (ValueType::kVoid == arg->GetReturnType()) {
      return Status::ParseError("If node argument cant be void type");
    }
    arg_types.emplace_back(arg->GetReturnType());
  }

  if (TypeHelper::IsNumericType(arg_types[1]) && TypeHelper::IsNumericType(arg_types[2])) {
    if_node.SetReturnType(TypeHelper::GetPromotedType(arg_types[1], arg_types[2]));
  } else if (arg_types[1] == arg_types[2]) {
    if_node.SetReturnType(arg_types[1]);
  } else {
    return Status::ParseError("If node is support for child type ", TypeHelper::TypeToString(arg_types[1]), " ",
                              TypeHelper::TypeToString(arg_types[2]));
  }
  return Status::OK();
}

Status Validator::Visit(SwitchNode& switch_node) {
  auto size = switch_node.GetArgs().size();
  if (0U == (size & 1)) {
    return Status::ParseError("Switch node must has odd number of arguments");
  }
  std::vector<ValueType> arg_types;
  arg_types.reserve(size);
  for (const auto& arg : switch_node.GetArgs()) {
    JF_RETURN_NOT_OK(arg->Accept(this));
    if (ValueType::kVoid == arg->GetReturnType()) {
      return Status::ParseError("Switch node argument cant be void type");
    }
    arg_types.emplace_back(arg->GetReturnType());
  }

  ValueType basic_type = ValueType::kUnknown;
  ValueType list_type = ValueType::kUnknown;
  for (std::size_t i = 0; i < size; i++) {
    if (i + 1 != size && i % 2 == 0 && !TypeHelper::IsNumericType(switch_node.GetArgs()[i]->GetReturnType())) {
      // TODO(victorika): maybe condition can support complex struct
      return Status::ParseError(
          "Unspported type: ", TypeHelper::TypeToString(switch_node.GetArgs()[i]->GetReturnType()),
          " in switch condition");
    }
    if (i % 2 != 0 || i + 1 == size) {
      if (TypeHelper::IsNumericType(switch_node.GetArgs()[i]->GetReturnType())) {
        basic_type = TypeHelper::GetPromotedType(basic_type, switch_node.GetArgs()[i]->GetReturnType());
      } else {
        if (list_type != ValueType::kUnknown && list_type != switch_node.GetArgs()[i]->GetReturnType()) {
          return Status::ParseError("Unspported different type in switch: ",
                                    TypeHelper::TypeToString(switch_node.GetArgs()[i]->GetReturnType()));
        }
        list_type = switch_node.GetArgs()[i]->GetReturnType();
      }
    }
  }

  if (list_type != ValueType::kUnknown && basic_type != ValueType::kUnknown) {
    return Status::ParseError("Unspported different type in switch");
  }
  switch_node.SetReturnType(basic_type == ValueType::kUnknown ? list_type : basic_type);

  return Status::OK();
}

Status Validator::Visit(IfBlockNode& if_block_node) {
  const auto& args = if_block_node.GetArgs();
  int num_args = static_cast<int>(args.size());
  if (num_args < 2) {
    return Status::ParseError("If block must have at least a condition and a body");
  }
  bool has_else = (num_args % 2 != 0);
  int num_branches = num_args / 2;

  auto type_snapshot = type_scope_stack_.Snapshot();

  std::unordered_map<std::string, ValueType> all_shadowed;

  for (int i = 0; i < num_branches; ++i) {
    int cond_idx = i * 2;
    int body_idx = (i * 2) + 1;

    JF_RETURN_NOT_OK(args[cond_idx]->Accept(this));
    if (!TypeHelper::IsNumericType(args[cond_idx]->GetReturnType())) {
      return Status::ParseError("If block condition must be numeric type, got ",
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
      return Status::ParseError("If block body must be void type, got ",
                                TypeHelper::TypeToString(args[body_idx]->GetReturnType()));
    }
    auto shadowed = type_scope_stack_.GetShadowed();
    for (const auto& [name, new_type] : shadowed) {
      auto it = type_snapshot.find(name);
      if (it != type_snapshot.end() && it->second != new_type) {
        return Status::ParseError("Variable '", name, "' type mismatch in if block branch: original type is ",
                                  TypeHelper::TypeToString(it->second), ", but assigned ",
                                  TypeHelper::TypeToString(new_type));
      }
      all_shadowed[name] = new_type;
    }
    type_scope_stack_.PopScope();
  }

  if (has_else) {
    // Else branch scope isolation: same as the condition branches above —
    // PushScope, execute, GetShadowed, then PopScope.
    type_scope_stack_.PushScope();
    JF_RETURN_NOT_OK(args[num_args - 1]->Accept(this));
    if (args[num_args - 1]->GetReturnType() != ValueType::kVoid) {
      return Status::ParseError("If block else body must be void type, got ",
                                TypeHelper::TypeToString(args[num_args - 1]->GetReturnType()));
    }
    auto shadowed = type_scope_stack_.GetShadowed();
    for (const auto& [name, new_type] : shadowed) {
      auto it = type_snapshot.find(name);
      if (it != type_snapshot.end() && it->second != new_type) {
        return Status::ParseError("Variable '", name, "' type mismatch in if block else branch: original type is ",
                                  TypeHelper::TypeToString(it->second), ", but assigned ",
                                  TypeHelper::TypeToString(new_type));
      }
      all_shadowed[name] = new_type;
    }
    type_scope_stack_.PopScope();
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
    return Status::ParseError("Variable not found: ", ref_node.GetName());
  }
  ref_node.SetReturnType(type);
  return Status::OK();
}

}  // namespace jitfusion
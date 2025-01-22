/*
 * @Author: victorika
 * @Date: 2025-01-14 14:55:53
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-22 15:40:46
 */
#pragma once

#include <memory>
#include <string>
#include "status.h"
#include "type.h"

namespace jitfusion {

class ExecNode;
class Visitor;

/**
 * This is the base class node of the execution engine.
 */
class ExecNode {
 public:
  virtual ~ExecNode() = default;
  std::string ToString(const std::string& prefix = "");
  virtual Status Accept(Visitor* visitor) = 0;
  virtual ExecNodeType GetExecNodeType() = 0;

  [[nodiscard]] ValueType GetReturnType() const { return return_type_; }
  void SetReturnType(ValueType return_type) { return_type_ = return_type; }

 private:
  virtual std::string ToStringImpl(const std::string& prefix);

  ValueType return_type_{ValueType::kUnknown};
};

class EntryArgumentNode : public ExecNode {
 public:
  Status Accept(Visitor* visitor) override;
  ExecNodeType GetExecNodeType() override;

 private:
  std::string ToStringImpl(const std::string& prefix) override;
};

class ExecContextNode : public ExecNode {
 public:
  Status Accept(Visitor* visitor) override;
  ExecNodeType GetExecNodeType() override;

 private:
  std::string ToStringImpl(const std::string& prefix) override;
};

class ConstantValueNode : public ExecNode {
 public:
  ConstantValueNode() = delete;
  explicit ConstantValueNode(ConstantValueType val) : val_(std::move(val)) {}
  Status Accept(Visitor* visitor) override;
  ExecNodeType GetExecNodeType() override;
  [[nodiscard]] const ConstantValueType& GetVal() const { return val_; }

 private:
  std::string ToStringImpl(const std::string& prefix) override;

  ConstantValueType val_;
};

class ConstantListValueNode : public ExecNode {
 public:
  ConstantListValueNode() = delete;
  explicit ConstantListValueNode(ConstantListValueType val_list) {
    sort_val_list_ = val_list;
    std::visit([](auto& v_list) { std::sort(v_list.begin(), v_list.end()); }, sort_val_list_);
    val_list_ = std::move(val_list);
  }
  Status Accept(Visitor* visitor) override;
  ExecNodeType GetExecNodeType() override;

  [[nodiscard]] const ConstantListValueType& GetValList() const { return val_list_; }
  [[nodiscard]] const ConstantListValueType& GetSortValList() const { return sort_val_list_; }

 private:
  std::string ToStringImpl(const std::string& prefix) override;

  ConstantListValueType val_list_;
  ConstantListValueType sort_val_list_;
};

class UnaryOPNode : public ExecNode {
 public:
  UnaryOPNode() = delete;
  UnaryOPNode(UnaryOPType op, std::unique_ptr<ExecNode> child) : op_(op), child_(std::move(child)) {}
  Status Accept(Visitor* visitor) override;
  ExecNodeType GetExecNodeType() override;

  [[nodiscard]] ExecNode* GetChild() const { return child_.get(); }
  [[nodiscard]] UnaryOPType GetOp() const { return op_; }

 private:
  std::string ToStringImpl(const std::string& prefix) override;

  UnaryOPType op_;
  std::unique_ptr<ExecNode> child_;
};

class BinaryOPNode : public ExecNode {
 public:
  BinaryOPNode() = delete;
  BinaryOPNode(BinaryOPType op, std::unique_ptr<ExecNode> left, std::unique_ptr<ExecNode> right)
      : op_(op), left_(std::move(left)), right_(std::move(right)) {}
  Status Accept(Visitor* visitor) override;
  ExecNodeType GetExecNodeType() override;

  [[nodiscard]] ExecNode* GetLeft() const { return left_.get(); }
  [[nodiscard]] ExecNode* GetRight() const { return right_.get(); }

  [[nodiscard]] BinaryOPType GetOp() const { return op_; }

 private:
  std::string ToStringImpl(const std::string& prefix) override;

  BinaryOPType op_;
  std::unique_ptr<ExecNode> left_, right_;
};

class FunctionNode : public ExecNode {
 public:
  FunctionNode() = delete;
  FunctionNode(std::string func_name, std::vector<std::unique_ptr<ExecNode>> args)
      : func_name_(std::move(func_name)), args_(std::move(args)) {}
  Status Accept(Visitor* visitor) override;
  ExecNodeType GetExecNodeType() override;
  void AppendArgs(std::unique_ptr<ExecNode>&& arg);

  [[nodiscard]] std::string GetFuncName() const { return func_name_; }
  [[nodiscard]] const std::vector<std::unique_ptr<ExecNode>>& GetArgs() const { return args_; }

 private:
  std::string ToStringImpl(const std::string& prefix) override;

  std::string func_name_;
  std::vector<std::unique_ptr<ExecNode>> args_;
};

class NoOPNode : public ExecNode {
 public:
  NoOPNode() = delete;
  explicit NoOPNode(std::vector<std::unique_ptr<ExecNode>> args) : args_(std::move(args)) {}
  Status Accept(Visitor* visitor) override;
  ExecNodeType GetExecNodeType() override;
  void AppendArgs(std::unique_ptr<ExecNode>&& arg);

  [[nodiscard]] const std::vector<std::unique_ptr<ExecNode>>& GetArgs() const { return args_; }

 private:
  std::string ToStringImpl(const std::string& prefix) override;

  std::vector<std::unique_ptr<ExecNode>> args_;
};

class Visitor {
 public:
  virtual ~Visitor() = default;

  virtual Status Visit(EntryArgumentNode& entry_argument_node) = 0;
  virtual Status Visit(ExecContextNode& exec_context_node) = 0;
  virtual Status Visit(ConstantValueNode& const_node) = 0;
  virtual Status Visit(ConstantListValueNode& const_list_node) = 0;
  virtual Status Visit(UnaryOPNode& unary_op_node) = 0;
  virtual Status Visit(BinaryOPNode& binary_op_node) = 0;
  virtual Status Visit(FunctionNode& function_node) = 0;
  virtual Status Visit(NoOPNode& no_op_node) = 0;
};

}  // namespace jitfusion
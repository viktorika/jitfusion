/*
 * @Author: victorika
 * @Date: 2025-04-09 15:44:37
 * @Last Modified by: victorika
 * @Last Modified time: 2025-04-16 16:25:25
 */
#pragma once

#include <memory>
#include <unordered_map>
#include "exec_node.h"
#include "location.hh"
#include "status.h"

namespace athena {

class Parser;

using jitfusion::ExecNode;
using jitfusion::Status;

struct Statement {
  Statement() = default;
  Statement(std::string vname, std::unique_ptr<jitfusion::ExecNode> exp)
      : var_name(std::move(vname)), expression(std::move(exp)), has_dependency(false) {}
  explicit Statement(std::unique_ptr<jitfusion::ExecNode> exp) : expression(std::move(exp)), has_dependency(false) {}
  ~Statement() = default;
  Statement(Statement&& s) = default;
  Statement& operator=(Statement&& other) = default;

  std::string var_name;
  std::unique_ptr<jitfusion::ExecNode> expression;
  bool has_dependency;
};

class ProgramAstBuilder {
 public:
  Status BuildProgram(const std::string& code, std::unique_ptr<ExecNode>* result);
  Status BuildProgram(const std::vector<std::string>& codes, std::unique_ptr<ExecNode>* result);

  std::unique_ptr<ExecNode> MakeRefNode(const std::string& var_name);
  void AddStatement(Statement statement);

  location& GetLocation() { return location_; }

 private:
  Status Scan(const std::string& code);

  std::vector<Statement> statements_;
  std::unordered_map<std::string, uint32_t> var2index_;
  std::string parser_error_message_;
  std::string custom_error_message_;
  location location_;

  friend class Parser;
};

}  // namespace athena
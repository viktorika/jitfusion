/*
 * @Author: victorika
 * @Date: 2025-04-09 15:44:37
 * @Last Modified by: victorika
 * @Last Modified time: 2025-04-10 15:13:53
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
      : var_name(std::move(vname)), expression(std::move(exp)) {}
  ~Statement() = default;
  Statement(Statement&& s) noexcept {
    var_name = std::move(s.var_name);
    expression = std::move(s.expression);
  }
  Statement& operator=(Statement&& other) noexcept {
    if (this != &other) {
      var_name = std::move(other.var_name);
      expression = std::move(other.expression);
    }
    return *this;
  }

  std::string var_name;
  std::unique_ptr<jitfusion::ExecNode> expression;
};

class ProgramAstBuilder {
 public:
  Status BuildProgram(const std::string& code, std::unique_ptr<ExecNode>* result);

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
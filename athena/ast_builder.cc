/*
 * @Author: victorika
 * @Date: 2025-04-09 15:44:50
 * @Last Modified by: victorika
 * @Last Modified time: 2025-04-10 15:21:42
 */
#include "ast_builder.h"
#include <exec_node.h>
#include <memory>
#include <string>
#include <vector>
#include "status.h"

namespace athena {

Status ProgramAstBuilder::BuildProgram(const std::string& code, std::unique_ptr<ExecNode>* result) {
  statements_.clear();
  var2index_.clear();
  parser_error_message_.clear();
  custom_error_message_.clear();
  location_.initialize(&code);
  if (auto st = Scan(code); !st.ok()) {
    return st;
  }
  if (!custom_error_message_.empty()) {
    return Status::ParseError(custom_error_message_);
  }
  *result = std::move(statements_.back().expression);
  return Status::OK();
}

std::unique_ptr<ExecNode> ProgramAstBuilder::MakeRefNode(const std::string& var_name) {
  if (auto it = var2index_.find(var_name); it != var2index_.end()) {
    return statements_[it->second].expression->Clone();
  }
  custom_error_message_ = "Variable not found: " + var_name;
  return nullptr;
}

void ProgramAstBuilder::AddStatement(Statement statement) {
  if (auto it = var2index_.find(statement.var_name); it != var2index_.end()) {
    custom_error_message_ = "Duplicate variable name: " + statement.var_name;
    return;
  }
  statements_.emplace_back(std::move(statement));
  var2index_[statements_.back().var_name] = statements_.size() - 1;
}

}  // namespace athena
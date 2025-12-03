/*
 * @Author: victorika
 * @Date: 2025-04-09 15:44:50
 * @Last Modified by: victorika
 * @Last Modified time: 2025-04-16 16:25:46
 */
#include "ast_builder.h"
#include <memory>
#include <string>
#include <vector>
#include "exec_node.h"
#include "status.h"

namespace athena {

using jitfusion::NoOPNode;

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

Status ProgramAstBuilder::BuildProgram(const std::vector<std::string>& codes, std::unique_ptr<ExecNode>* result) {
  std::vector<std::unique_ptr<ExecNode>> args;
  for (const auto& code : codes) {
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
    for (auto& statement : statements_) {
      if (!statement.has_dependency) {
        args.emplace_back(std::move(statement.expression));
      }
    }
  }
  auto root = std::unique_ptr<ExecNode>(new NoOPNode(std::move(args)));
  *result = std::move(root);
  return Status::OK();
}

std::unique_ptr<ExecNode> ProgramAstBuilder::MakeRefNode(const std::string& var_name) {
  if (auto it = var2index_.find(var_name); it != var2index_.end()) {
    statements_[it->second].has_dependency = true;
    return statements_[it->second].expression->Clone();
  }
  custom_error_message_ = "Variable not found: " + var_name;
  return nullptr;
}

void ProgramAstBuilder::AddStatement(Statement statement) {
  statements_.emplace_back(std::move(statement));
  const auto& var_name = statements_.back().var_name;
  if (var_name.empty()) {
    return;
  }
  var2index_[var_name] = statements_.size() - 1;
}

}  // namespace athena
/*
 * @Author: victorika
 * @Date: 2025-04-09 15:44:50
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-07 15:23:05
 */
#include "ast_builder.h"
#include <memory>
#include <string>
#include <vector>
#include "diagnostic.h"
#include "exec_node.h"
#include "status.h"

namespace athena {

using jitfusion::NoOPNode;
using jitfusion::RefNode;

Status ProgramAstBuilder::BuildExpression(const std::string& code, std::unique_ptr<ExecNode>* result) {
  statements_.clear();
  var2index_.clear();
  parser_error_message_.clear();
  custom_error_message_.clear();
  source_code_ = code;
  build_mode_ = BuildMode::kExpression;
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

Status ProgramAstBuilder::BuildPipeline(const std::string& code, std::unique_ptr<ExecNode>* result) {
  std::vector<std::string> names;
  std::vector<std::unique_ptr<ExecNode>> args;
  statements_.clear();
  var2index_.clear();
  parser_error_message_.clear();
  custom_error_message_.clear();
  source_code_ = code;
  build_mode_ = BuildMode::kPipeline;
  location_.initialize(&code);
  if (auto st = Scan(code); !st.ok()) {
    return st;
  }
  if (!custom_error_message_.empty()) {
    return Status::ParseError(custom_error_message_);
  }
  names.reserve(statements_.size());
  args.reserve(statements_.size());
  for (auto& statement : statements_) {
    names.emplace_back(std::move(statement.var_name));
    args.emplace_back(std::move(statement.expression));
  }
  auto root = std::unique_ptr<ExecNode>(new NoOPNode(std::move(names), std::move(args)));
  *result = std::move(root);
  return Status::OK();
}

std::unique_ptr<ExecNode> ProgramAstBuilder::MakeRefNode(const std::string& var_name, int begin_line, int begin_col,
                                                         int end_line, int end_col) {
  if (auto it = var2index_.find(var_name); it != var2index_.end()) {
    statements_[it->second].has_dependency = true;
    if (build_mode_ == BuildMode::kPipeline) {
      return std::make_unique<RefNode>(var_name);
    }
    return statements_[it->second].expression->Clone();
  }
  for (auto it = block_stack_.rbegin(); it != block_stack_.rend(); ++it) {
    if (auto found = it->var2index.find(var_name); found != it->var2index.end()) {
      it->statements[found->second].has_dependency = true;
      if (build_mode_ == BuildMode::kPipeline) {
        return std::make_unique<RefNode>(var_name);
      }
      return it->statements[found->second].expression->Clone();
    }
  }
  // Variable not found — emit a diagnostic-style error with a source snippet
  // and caret underline, anchored at the identifier's location.
  jitfusion::Diagnostic diag(jitfusion::Diagnostic::Level::kError, "variable '" + var_name + "' is not defined",
                             jitfusion::SourceLocation{source_code_, begin_line, begin_col, end_line, end_col});
  custom_error_message_ = diag.Render();
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

void ProgramAstBuilder::EnterBlock() {
  block_stack_.emplace_back(std::move(statements_), std::move(var2index_));
  statements_.clear();
  var2index_.clear();
}

std::unique_ptr<ExecNode> ProgramAstBuilder::LeaveBlock() {
  std::vector<std::string> names;
  std::vector<std::unique_ptr<ExecNode>> args;
  names.reserve(statements_.size());
  args.reserve(statements_.size());
  for (auto& stmt : statements_) {
    names.emplace_back(std::move(stmt.var_name));
    args.emplace_back(std::move(stmt.expression));
  }
  auto block_node = std::unique_ptr<ExecNode>(new NoOPNode(std::move(names), std::move(args)));

  auto& outer = block_stack_.back();
  statements_ = std::move(outer.statements);
  var2index_ = std::move(outer.var2index);
  block_stack_.pop_back();

  return block_node;
}

}  // namespace athena
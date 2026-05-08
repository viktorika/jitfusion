/*
 * @Author: victorika
 * @Date: 2025-04-09 15:44:26
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-08 10:00:00
 */
#include "athena.h"
#include "ast_builder.h"
#include "batch_exec_engine.h"
#include "exec_engine.h"
#include "function_registry.h"
#include "pipeline_grouper.h"
#include "status.h"

namespace athena {

Status AthenaExpression::Compile(const std::string& code, const std::unique_ptr<FunctionRegistry>& func_registry) {
  ProgramAstBuilder ast_builder;
  std::unique_ptr<ExecNode> program_ast;
  JF_RETURN_NOT_OK(ast_builder.BuildExpression(code, &program_ast));
  return engine_.Compile(program_ast, func_registry);
}

Status AthenaPipeline::Compile(const std::vector<std::string>& code,
                               const std::unique_ptr<FunctionRegistry>& func_registry) {
  ProgramAstBuilder ast_builder;
  std::vector<std::unique_ptr<ExecNode>> program_asts;
  for (const auto& code_str : code) {
    std::unique_ptr<ExecNode> program_ast;
    JF_RETURN_NOT_OK(ast_builder.BuildPipeline(code_str, &program_ast));
    program_asts.emplace_back(std::move(program_ast));
  }
  PipelineGrouper pipeline_grouper;
  std::vector<std::unique_ptr<ExecNode>> grouped_program_asts = pipeline_grouper.Group(program_asts);
  return engine_.Compile(grouped_program_asts, func_registry);
}

}  // namespace athena

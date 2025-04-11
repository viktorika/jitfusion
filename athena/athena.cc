/*
 * @Author: victorika
 * @Date: 2025-04-09 15:44:26
 * @Last Modified by: victorika
 * @Last Modified time: 2025-04-10 15:02:48
 */
#include "athena.h"
#include "ast_builder.h"
#include "exec_engine.h"
#include "function_registry.h"
#include "status.h"

namespace athena {

Status Athena::Compile(const std::string& code, const std::unique_ptr<FunctionRegistry>& func_registry) {
  ProgramAstBuilder ast_builder;
  std::unique_ptr<ExecNode> program_ast;
  JF_RETURN_NOT_OK(ast_builder.BuildProgram(code, &program_ast));
  return exec_engine_.Compile(program_ast, func_registry);
}

Status Athena::Execute(void* entry_arguments, RetType* result) { return exec_engine_.Execute(entry_arguments, result); }

}  // namespace athena
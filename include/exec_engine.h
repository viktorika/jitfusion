/*
 * @Author: victorika
 * @Date: 2025-01-15 10:48:46
 * @Last Modified by: victorika
 * @Last Modified time: 2025-04-16 11:32:08
 */
#pragma once

#include <memory>
#include "arena.h"
#include "exec_node.h"
#include "function_registry.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

struct ExecContext {
  explicit ExecContext(int64_t alloc_min_chunk_size) : arena(alloc_min_chunk_size) {}
  Arena arena;

  void Clear() { arena.Reset(); }
};

struct ExecEngineOption {
  int64_t const_value_arena_alloc_min_chunk_size{4096};
  int64_t exec_ctx_arena_alloc_min_chunk_size{4096};
};

class ExecEngine {
 public:
  explicit ExecEngine(ExecEngineOption option = {});
  ~ExecEngine() = default;
  ExecEngine(const ExecEngine&) = delete;
  ExecEngine& operator=(const ExecEngine&) = delete;

  Status Compile(const std::unique_ptr<ExecNode>& exec_node, const std::unique_ptr<FunctionRegistry>& func_registry);
  // Applicable to scenarios with similar expressions, a result is returned through the root node.
  // If you need to optimize the memory allocation issue of ExecContext, you can use the function passed to ExecContext.
  Status Execute(void* entry_arguments, RetType* result);
  Status Execute(ExecContext& exec_ctx, void* entry_arguments, RetType* result);

  // Applicable to more complex scenarios, users need to use an output node and a custom store function to write data,
  // and it will not return data from the root node. and root node must be the NoOpNode.
  // If you need to optimize the memory allocation issue of ExecContext, you can use the function passed to ExecContext.
  Status Execute(void* entry_arguments, void* result);
  Status Execute(ExecContext& exec_ctx, void* entry_arguments, void* result);

  [[nodiscard]] std::string_view GetIRCode() const { return ir_code_; }

 private:
  Arena const_value_arena_;
  llvm::Expected<std::unique_ptr<llvm::orc::LLJIT>> jit_;
  char* entry_func_ptr_;
  ValueType ret_type_;
  ExecEngineOption option_;
  std::string ir_code_;
};

}  // namespace jitfusion
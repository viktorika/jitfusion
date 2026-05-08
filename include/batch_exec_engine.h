/*
 * @Author: victorika
 * @Date: 2026-05-07 18:00:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 18:00:00
 */
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "exec_node.h"
#include "exec_options.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

class JitCore;  // private, defined in src/jit_core.h

// Batch-oriented counterpart of ExecEngine.
//
// Use this class when you want to compile several independent expressions
// into a single JIT module so they can share a single set of optimization
// passes, memory pages, and entry-argument layout. Each compiled
// expression keeps its own return type and is addressed by index.
//
// Picking between ExecEngine and BatchExecEngine:
//
//   * ExecEngine       — one expression per JIT. Simplest; prefer this
//                        when you do not have N >= 2 related expressions.
//   * BatchExecEngine  — N >= 1 expressions share one JIT. Slightly
//                        lower per-call overhead once N is large and
//                        lets ORC share .rodata constants across entries.
//
// Thread-safety (mirrors ExecEngine):
//   - Compile() is NOT thread-safe.
//   - After a successful Compile(), the ExecuteAt / ExecuteAll overloads
//     are safe to invoke concurrently across threads as long as each
//     thread uses its own ExecContext.
//   - A single ExecContext must never be shared across threads.
class BatchExecEngine {
 public:
  explicit BatchExecEngine(ExecEngineOption option = {});
  ~BatchExecEngine();
  BatchExecEngine(const BatchExecEngine&) = delete;
  BatchExecEngine& operator=(const BatchExecEngine&) = delete;

  // --- Compile -------------------------------------------------------------
  //
  // Compile N >= 1 expression roots into N entry functions sharing one
  // JIT module. Each root may have its own return type; query the i-th
  // return type via GetReturnType(i).
  Status Compile(const std::vector<std::unique_ptr<ExecNode>>& exec_nodes,
                 const std::unique_ptr<FunctionRegistry>& func_registry);

  // --- Execute (index) -----------------------------------------------------
  //
  // Execute the i-th compiled entry. The RetType* overloads box the
  // result into the std::variant RetType and are the common case.
  // The void* overloads are for entries whose root is a NoOPNode and
  // that write results through an OutputNode + store function instead.
  Status ExecuteAt(size_t index, void* entry_arguments, RetType* result);
  Status ExecuteAt(size_t index, ExecContext& exec_ctx, void* entry_arguments, RetType* result);
  Status ExecuteAt(size_t index, void* entry_arguments, void* result);
  Status ExecuteAt(size_t index, ExecContext& exec_ctx, void* entry_arguments, void* result);

  // --- Execute (all) -------------------------------------------------------
  //
  // Convenience: run every compiled entry in order and collect results.
  // `results` is resized to the function count.
  Status ExecuteAll(void* entry_arguments, std::vector<RetType>* results);
  Status ExecuteAll(ExecContext& exec_ctx, void* entry_arguments, std::vector<RetType>* results);
  // For void-returning entries only (root = NoOPNode). `results` is passed
  // through to the user store functions; its interpretation is up to the
  // caller.
  Status ExecuteAll(void* entry_arguments, void* results);
  Status ExecuteAll(ExecContext& exec_ctx, void* entry_arguments, void* results);

  // --- Introspection -------------------------------------------------------
  [[nodiscard]] size_t GetFunctionCount() const { return entry_func_ptrs_.size(); }
  [[nodiscard]] ValueType GetReturnType(size_t index) const { return ret_types_[index]; }
  [[nodiscard]] std::string_view GetIRCode() const;

  // --- Persistence ---------------------------------------------------------
  //
  // SaveCompiled packs the JIT-produced relocatable object bytes plus a
  // small header (target triple, LLVM version, FP mode, per-entry return
  // types, ...) into a self-contained byte string. LoadCompiled is an
  // alternative entry point to Compile that accepts such a blob plus a
  // FunctionRegistry.
  //
  // Compatibility contract (same as ExecEngine):
  //   * LLVM version / target triple / CPU must match exactly by
  //     default. Individual checks can be relaxed through
  //     LoadCompiledOptions if the caller knows the divergence is
  //     safe (see that struct for the hazards).
  //   * FunctionRegistry must re-register every C function the saved
  //     expression depends on under the same FunctionSignature.
  //   * GetIRCode() is empty after LoadCompiled — IR is never rebuilt.
  //
  // SaveCompiled requires ExecEngineOption::enable_save_compiled=true at
  // the time of the original Compile(); otherwise no object bytes are
  // captured and SaveCompiled returns an error. Blobs saved here can
  // ONLY be loaded by BatchExecEngine::LoadCompiled — loading a batch
  // blob into ExecEngine (or vice versa) fails cleanly with a mode
  // mismatch message.
  Status SaveCompiled(std::string* out) const;
  Status LoadCompiled(std::string_view bytes, const std::unique_ptr<FunctionRegistry>& func_registry,
                      const LoadCompiledOptions& opts = {});

 private:
  std::unique_ptr<JitCore> core_;
  std::vector<char*> entry_func_ptrs_;
  std::vector<ValueType> ret_types_;
};

}  // namespace jitfusion

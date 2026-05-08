/*
 * @Author: victorika
 * @Date: 2025-01-15 10:48:46
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 18:30:00
 */
#pragma once

#include <memory>
#include <string>
#include <string_view>
#include "exec_node.h"
#include "exec_options.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

class JitCore;  // private, defined in src/jit_core.h

// ExecEngine compiles and runs a single expression.
//
// For multiple related expressions sharing one JIT module, use
// BatchExecEngine (include/batch_exec_engine.h) instead.
//
// Thread-safety contract:
// - Compile() is NOT thread-safe and must not run concurrently with any other
//   method on the same ExecEngine instance. It mutates the engine's internal
//   JIT state, const-value arena and function-pointer table.
// - After a successful Compile(), the Execute* overloads are safe to invoke
//   concurrently from multiple threads on the same ExecEngine, provided each
//   thread uses its own ExecContext. During execution the engine itself is
//   only read; all per-call mutable state (arena, error list, intermediate
//   allocations) lives inside ExecContext.
// - Overloads that do not take an explicit ExecContext internally construct a
//   fresh one on every call. They are thread-safe but pay per-call allocation
//   cost; prefer the ExecContext& overloads on hot paths so the arena can be
//   reused.
// - A single ExecContext must NOT be shared across threads. Recommended pattern
//   for parallel execution: 1 ExecEngine + N ExecContexts (one per worker).
class ExecEngine {
 public:
  explicit ExecEngine(ExecEngineOption option = {});
  ~ExecEngine();
  ExecEngine(const ExecEngine&) = delete;
  ExecEngine& operator=(const ExecEngine&) = delete;

  // --- Compile -------------------------------------------------------------
  Status Compile(const std::unique_ptr<ExecNode>& exec_node, const std::unique_ptr<FunctionRegistry>& func_registry);

  // --- Execute -------------------------------------------------------------
  //
  // RetType* overloads: box the result into the std::variant RetType.
  // void*    overloads: for entries whose root is a NoOPNode; data is
  //                     written by the user store function rather than
  //                     returned. `result` is forwarded to that store.
  Status Execute(void* entry_arguments, RetType* result);
  Status Execute(ExecContext& exec_ctx, void* entry_arguments, RetType* result);
  Status Execute(void* entry_arguments, void* result);
  Status Execute(ExecContext& exec_ctx, void* entry_arguments, void* result);

  // --- Introspection -------------------------------------------------------
  [[nodiscard]] std::string_view GetIRCode() const;

  // --- Persistence ---------------------------------------------------------
  //
  // SaveCompiled / LoadCompiled: persist a relocatable object-file blob
  // plus a small header (engine mode, return type, target triple, LLVM
  // version, FP mode, ...) so callers can skip the expensive O3 pipeline
  // on subsequent runs.
  //
  // Compatibility contract:
  //   * LLVM version / target triple / CPU must match exactly by
  //     default. Individual checks can be relaxed through
  //     LoadCompiledOptions if the caller knows the divergence is
  //     safe (see that struct for the hazards).
  //   * The FunctionRegistry passed to LoadCompiled must re-register
  //     every C function referenced by the original expression under
  //     the same FunctionSignature; addresses may differ across
  //     processes (the whole point of relocatable globals), but
  //     names + param/ret types must match.
  //   * GetIRCode() is NOT populated after LoadCompiled — IR is never
  //     rebuilt. This is intentional.
  //
  // SaveCompiled requires ExecEngineOption::enable_save_compiled=true at
  // the time of Compile(); otherwise no object bytes are captured and
  // SaveCompiled returns an error. Blobs saved by ExecEngine can ONLY
  // be loaded by ExecEngine::LoadCompiled — loading a batch blob into
  // ExecEngine (or vice versa) fails cleanly with a mode mismatch
  // message.
  Status SaveCompiled(std::string* out) const;
  Status LoadCompiled(std::string_view bytes, const std::unique_ptr<FunctionRegistry>& func_registry,
                      const LoadCompiledOptions& opts = {});

 private:
  std::unique_ptr<JitCore> core_;
  char* entry_func_ptr_{nullptr};
  ValueType ret_type_{ValueType::kUnknown};
};

}  // namespace jitfusion

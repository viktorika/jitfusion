/*
 * @Author: victorika
 * @Date: 2025-01-15 10:48:46
 * @Last Modified by: victorika
 * @Last Modified time: 2026-03-31 14:45:31
 */
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "arena.h"
#include "exec_node.h"
#include "function_registry.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

// Per-call mutable state used during Execute*.
// An ExecContext holds a memory arena (for values whose lifetime equals one
// execution) and an error list (populated by user-supplied C functions via
// AddError()). It is NOT thread-safe: a single ExecContext must never be shared
// across threads. For parallel execution, give each worker thread its own
// ExecContext; a single ExecEngine can be shared by all of them after Compile().
// Within one thread, reusing an ExecContext across calls (via the
// `Execute(ExecContext&, ...)` overloads) avoids re-allocating the arena on
// each call.
struct ExecContext {
  explicit ExecContext(int64_t alloc_min_chunk_size) : arena(alloc_min_chunk_size) {}
  Arena arena;
  std::vector<std::string> errors;

  void Clear() {
    arena.Reset();
    errors.clear();
  }

  [[nodiscard]] bool HasErrors() const { return !errors.empty(); }

  [[nodiscard]] std::string GetErrorMessage() const {
    std::string msg;
    for (size_t i = 0; i < errors.size(); ++i) {
      if (i > 0) {
        msg += "; ";
      }
      msg += errors[i];
    }
    return msg;
  }

  void AddError(std::string msg) { errors.emplace_back(std::move(msg)); }
};

// Floating-point semantics to request from the JIT code generator.
//
// kStrict
//   IEEE-754 conformant: NaN / +-Inf / signed zero / rounding mode are all
//   preserved. No fused multiply-add reassociation, no algebraic rewrites
//   that could change bit-for-bit results. Pick this for finance / risk /
//   anything where reproducibility across machines & LLVM versions matters.
//
// kFast
//   Equivalent to enabling `-ffast-math` globally:
//     * fused multiply-add (AllowFPOpFusion = Fast)
//     * unsafe algebraic transforms (UnsafeFPMath = true), which implies
//       NaN / Inf / signed-zero may be ignored and reassociation is allowed.
//   Typically 1.3x - 2x faster on FP-heavy list kernels, but results can
//   differ slightly from strict IEEE. Pick this for analytics / ML / search
//   ranking where throughput dominates.
//
// The default is kFast for backwards compatibility with releases prior to
// the introduction of this switch.
enum class FPMathMode : std::uint8_t {
  kStrict,
  kFast,
};

struct ExecEngineOption {
  int64_t const_value_arena_alloc_min_chunk_size{4096};
  int64_t exec_ctx_arena_alloc_min_chunk_size{4096};
  // If true, the fully optimized LLVM IR text will be captured during Compile and made available via GetIRCode().
  // Mainly for debugging: serializing a large Module to text is expensive (can be hundreds of KB and requires a
  // full Module walk), so it is disabled by default in production.
  bool dump_ir{false};
  // See FPMathMode above. Default stays kFast so that existing users'
  // observed numeric behavior does not change when they upgrade.
  FPMathMode fp_math_mode{FPMathMode::kFast};
};

// Thread-safety contract:
// - Compile() / BatchCompile() are NOT thread-safe and must not run concurrently
//   with any other method on the same ExecEngine instance. They mutate the
//   engine's internal JIT state, const-value arena and function-pointer table.
// - After a successful Compile() / BatchCompile(), the Execute* / ExecuteAt* /
//   ExecuteAll* overloads are safe to invoke concurrently from multiple threads
//   on the same ExecEngine, provided each thread uses its own ExecContext.
//   During execution the engine itself is only read; all per-call mutable state
//   (arena, error list, intermediate allocations) lives inside ExecContext.
// - Overloads that do not take an explicit ExecContext internally construct a
//   fresh one on every call. They are thread-safe but pay per-call allocation
//   cost; prefer the ExecContext& overloads on hot paths so the arena can be
//   reused.
// - A single ExecContext must NOT be shared across threads. Recommended pattern
//   for parallel execution: 1 ExecEngine + N ExecContexts (one per worker).
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

  // Batch compile multiple ASTs into separate functions within a single module.
  // Each ExecNode can have any return type. All functions share the same JIT and memory pages.
  Status BatchCompile(const std::vector<std::unique_ptr<ExecNode>>& exec_nodes,
                      const std::unique_ptr<FunctionRegistry>& func_registry);
  // Execute a specific batch-compiled function by index.
  // For void return type functions (NoOPNode root), use the void* result overload.
  Status ExecuteAt(size_t index, void* entry_arguments, RetType* result);
  Status ExecuteAt(size_t index, ExecContext& exec_ctx, void* entry_arguments, RetType* result);
  // For void return type functions only.
  Status ExecuteAt(size_t index, void* entry_arguments, void* result);
  Status ExecuteAt(size_t index, ExecContext& exec_ctx, void* entry_arguments, void* result);
  // Execute all batch-compiled functions.
  Status ExecuteAll(void* entry_arguments, std::vector<RetType>* results);
  Status ExecuteAll(ExecContext& exec_ctx, void* entry_arguments, std::vector<RetType>* results);
  // For void return type functions only.
  Status ExecuteAll(void* entry_arguments, void* results);
  Status ExecuteAll(ExecContext& exec_ctx, void* entry_arguments, void* results);

  [[nodiscard]] size_t GetBatchFunctionCount() const { return batch_entry_func_ptrs_.size(); }
  [[nodiscard]] ValueType GetBatchFunctionReturnType(size_t index) const { return batch_ret_types_[index]; }

  [[nodiscard]] std::string_view GetIRCode() const { return ir_code_; }

 private:
  Status CreateJitAndOptimize(const std::unique_ptr<FunctionRegistry>& func_registry,
                              std::unique_ptr<llvm::Module> owner, std::unique_ptr<llvm::LLVMContext> llvm_context);

  Arena const_value_arena_;
  llvm::Expected<std::unique_ptr<llvm::orc::LLJIT>> jit_;
  char* entry_func_ptr_;
  ValueType ret_type_;
  ExecEngineOption option_;
  std::string ir_code_;
  std::vector<char*> batch_entry_func_ptrs_;
  std::vector<ValueType> batch_ret_types_;
};

}  // namespace jitfusion
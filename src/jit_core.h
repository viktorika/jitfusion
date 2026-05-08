/*
 * @Author: victorika
 * @Date: 2026-05-07 18:00:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 18:00:00
 */
/*
 * Private, non-public helper shared by ExecEngine and BatchExecEngine.
 *
 * JitCore owns the LLJIT instance plus the small amount of per-compile
 * bookkeeping that is identical for single-expression and multi-expression
 * compilation:
 *   * LLJIT construction with the correct FP math options
 *   * Dynamic-library resolver wiring
 *   * Optional capture of the raw object bytes (for SaveCompiled)
 *   * Optional capture of the fully-optimized IR text (for GetIRCode)
 *   * Symbol lookup after materialization
 *   * Bulk compile helper that takes (N ExecNode roots, N symbol names)
 *     and returns (N function pointers, N return types).
 *
 * This header is PRIVATE (src/ only). Public headers must not include it
 * — it drags in LLVM and defeats the "minimal public surface" promise of
 * include/exec_engine.h.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "exec_node.h"
#include "exec_options.h"  // FPMathMode / ExecEngineOption
#include "function_registry.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

// Deterministic symbol-naming convention shared by Save and Load:
//   single mode            -> "entry"
//   batch mode entry i     -> "entry" + std::to_string(i)
extern const std::string kEntryFunctionName;

class JitCore {
 public:
  explicit JitCore(ExecEngineOption option) : option_(option) {}
  ~JitCore() = default;
  JitCore(const JitCore&) = delete;
  JitCore& operator=(const JitCore&) = delete;

  // Compile N ExecNode roots into N entry functions with the supplied
  // symbol names, optimize with the O3 pipeline, and resolve each symbol
  // to a host function pointer.
  //
  // On success:
  //   *out_func_ptrs receives N entry function pointers (same order)
  //   *out_ret_types receives the ValueType of each root's result
  //   GetIRCode() and (if enabled) the captured object bytes are
  //   populated as a side effect.
  //
  // On failure: the JitCore is left reset (no partial state).
  Status BuildAndLink(const std::vector<ExecNode*>& nodes, const std::vector<std::string>& symbol_names,
                      const std::unique_ptr<FunctionRegistry>& func_registry, std::vector<char*>* out_func_ptrs,
                      std::vector<ValueType>* out_ret_types);

  // Take a previously-serialized relocatable object buffer and install it
  // into a fresh LLJIT. Resolves each symbol in `symbol_names` to a host
  // function pointer. Used by LoadCompiled.
  //
  // Caller is responsible for having validated the object bytes' LLVM
  // version / target triple / CPU compatibility before calling this.
  Status LoadObject(std::string_view object_bytes, const std::vector<std::string>& symbol_names,
                    const std::unique_ptr<FunctionRegistry>& func_registry, std::vector<char*>* out_func_ptrs);

  // Drop the LLJIT and any captured bytes / IR text. Safe to call on a
  // never-compiled instance.
  void Reset();

  // Accessors used by SaveCompiled / GetIRCode at the public-class layer.
  [[nodiscard]] std::string_view GetIRCode() const { return ir_code_; }
  [[nodiscard]] std::string_view GetCompiledObjectBytes() const { return compiled_object_bytes_; }
  [[nodiscard]] const ExecEngineOption& GetOption() const { return option_; }

 private:
  // Shared between BuildAndLink and LoadObject: construct a fresh LLJIT
  // with the right FP options and add the process dylib resolver.
  Status CreateJit();

  ExecEngineOption option_;
  std::unique_ptr<llvm::orc::LLJIT> jit_;
  std::string ir_code_;
  std::string compiled_object_bytes_;
};

}  // namespace jitfusion

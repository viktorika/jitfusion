/*
 * @Author: victorika
 * @Date: 2026-05-07 18:30:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 18:30:00
 */
/*
 * Public configuration / runtime-context types shared by ExecEngine
 * and BatchExecEngine.
 *
 * Why these live in their own header (rather than inside exec_engine.h):
 *   * `ExecContext` is per-call mutable state; users construct one (or
 *     N — one per worker) and pass it to either engine class.
 *   * `ExecEngineOption` and `FPMathMode` parameterize compile-time
 *     behavior of either engine class.
 * They are fundamentally orthogonal to "single vs batch", so giving
 * them an independent home keeps each engine header focused on its
 * own class definition. This file is also small and LLVM-free, so it
 * is cheap for non-engine TUs (e.g. the serialization layer) to depend
 * on directly.
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "arena.h"

namespace jitfusion {

// Per-call mutable state used during Execute*.
// An ExecContext holds a memory arena (for values whose lifetime equals one
// execution) and an error list (populated by user-supplied C functions via
// AddError()). It is NOT thread-safe: a single ExecContext must never be
// shared across threads. For parallel execution, give each worker thread
// its own ExecContext; a single ExecEngine / BatchExecEngine can be shared
// by all of them after Compile().
// Within one thread, reusing an ExecContext across calls (via the
// `Execute(ExecContext&, ...)` overloads) avoids re-allocating the arena on
// each call. The engine resets the arena and clears the error list at the
// end of every Execute(ExecContext&, ...) call, so callers do NOT need to
// invoke Clear() manually between calls. As a consequence, any pointer
// returned through the arena (list / string payloads embedded in the
// previous result) becomes invalid as soon as the next Execute call
// returns — copy out anything you need to keep before issuing the next
// call.
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
//
// On-wire stability: kStrict and kFast are pinned to numeric values 0 and
// 1 respectively. The serialization format (compiled_artifact) writes
// the enum value as a single byte; reordering or renumbering members
// would silently break previously-saved artifacts. A static_assert pair
// in compiled_artifact.cc guards this.
enum class FPMathMode : std::uint8_t {
  kStrict = 0,
  kFast = 1,
};

struct ExecEngineOption {
  int64_t exec_ctx_arena_alloc_min_chunk_size{4096};
  // If true, the fully optimized LLVM IR text will be captured during
  // Compile and made available via GetIRCode(). Mainly for debugging:
  // serializing a large Module to text is expensive (can be hundreds of
  // KB and requires a full Module walk), so it is disabled by default in
  // production.
  bool dump_ir{false};
  // See FPMathMode above. Default stays kFast so that existing users'
  // observed numeric behavior does not change when they upgrade.
  FPMathMode fp_math_mode{FPMathMode::kFast};
  // If true, the raw compiled object bytes are captured during
  // Compile() and retained in the engine so that SaveCompiled() can
  // serialize them afterwards. Defaults to false because capturing
  // costs one memcpy and ~KBs-to-MBs of extra resident memory per
  // engine — pure waste for users who never persist compiled artifacts.
  //
  // Note on lifecycle: the captured bytes are released by the next
  // Compile() or when the engine is destroyed. There is deliberately
  // no "release now" API: SaveCompiled may legitimately be called
  // multiple times, so the engine keeps the bytes as long as the
  // compilation is live.
  //
  // Must be set before Compile(); flipping it after the fact does NOT
  // retroactively materialize bytes for an already-compiled engine —
  // you would need to re-Compile().
  bool enable_save_compiled{false};
};

}  // namespace jitfusion

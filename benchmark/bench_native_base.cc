// D. ExecContext arena reuse vs fresh.
// E. Batch execute (ExecuteAll / ExecuteAt loop).
// F. Hand-written native C++ baselines.
// See bench_compile.cc for the overview.

#include <numeric>

#include "benchmark/bench_common.h"

namespace {

using ::jitfusion::bench::CompileOrDie;
using ::jitfusion::bench::MakeLinearAddChain;
using ::jitfusion::bench::MakeListUnaryCall;
using ::jitfusion::bench::MakeRegistry;
using ::jitfusion::ExecContext;
using ::jitfusion::ExecEngine;
using ::jitfusion::ExecNode;
using ::jitfusion::RetType;

// =============================================================================
// D. ExecContext reuse — arena allocator cost
// =============================================================================
//
// Same AST, same engine, two call patterns:
//   - Reuse: one ExecContext is shared across iterations, Clear()'d each time
//     → arena memory from the first iteration is kept and reused.
//   - Fresh: a new ExecContext is constructed every iteration
//     → arena allocator starts from scratch, paying first-chunk malloc cost.
// The gap is the payoff of the ExecContext& Execute overload on hot paths.

void BM_Execute_ArenaReuse(benchmark::State& state) {
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeLinearAddChain(8), reg);
  ExecContext ctx(4096);
  for (auto _ : state) {
    ctx.Clear();
    RetType result;
    auto st = engine->Execute(ctx, nullptr, &result);
    benchmark::DoNotOptimize(result);
    if (!st.ok()) {
      state.SkipWithError("execute failed");
    }
  }
}
BENCHMARK(BM_Execute_ArenaReuse);

void BM_Execute_ArenaFresh(benchmark::State& state) {
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeLinearAddChain(8), reg);
  for (auto _ : state) {
    ExecContext ctx(4096);
    RetType result;
    auto st = engine->Execute(ctx, nullptr, &result);
    benchmark::DoNotOptimize(result);
    if (!st.ok()) {
      state.SkipWithError("execute failed");
    }
  }
}
BENCHMARK(BM_Execute_ArenaFresh);

// Same pair, but with a list aggregation that forces real arena usage
// (list materialisation + kernel scratch space), so the reuse / fresh gap
// is more visible than in the const-fold case above.
void BM_Execute_ListSum_ArenaReuse(benchmark::State& state) {
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListUnaryCall("Sum", 256), reg);
  ExecContext ctx(4096);
  for (auto _ : state) {
    ctx.Clear();
    RetType result;
    auto st = engine->Execute(ctx, nullptr, &result);
    benchmark::DoNotOptimize(result);
    if (!st.ok()) {
      state.SkipWithError("execute failed");
    }
  }
}
BENCHMARK(BM_Execute_ListSum_ArenaReuse);

void BM_Execute_ListSum_ArenaFresh(benchmark::State& state) {
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListUnaryCall("Sum", 256), reg);
  for (auto _ : state) {
    ExecContext ctx(4096);
    RetType result;
    auto st = engine->Execute(ctx, nullptr, &result);
    benchmark::DoNotOptimize(result);
    if (!st.ok()) {
      state.SkipWithError("execute failed");
    }
  }
}
BENCHMARK(BM_Execute_ListSum_ArenaFresh);

// =============================================================================
// E. Batch execute
// =============================================================================
//
// BatchCompile N ASTs once, then measure only the per-iteration execute
// cost. Two access patterns:
//   - ExecuteAll: hand the vector back in one call.
//   - ExecuteAtLoop: call ExecuteAt(i) in a loop on the benchmark side.
// The two should be close; a large gap would point at per-call overhead in
// the wrapper layer.

void BM_BatchExecute_All(benchmark::State& state) {
  const int n = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  std::vector<std::unique_ptr<ExecNode>> nodes;
  nodes.reserve(n);
  for (int i = 0; i < n; ++i) {
    nodes.emplace_back(MakeLinearAddChain(8));
  }
  ExecEngine engine;
  auto cs = engine.BatchCompile(nodes, reg);
  if (!cs.ok()) {
    state.SkipWithError("batch compile failed");
    return;
  }
  ExecContext ctx(4096);
  std::vector<RetType> results;
  for (auto _ : state) {
    ctx.Clear();
    results.clear();
    auto st = engine.ExecuteAll(ctx, nullptr, &results);
    benchmark::DoNotOptimize(results);
    if (!st.ok()) {
      state.SkipWithError("execute all failed");
    }
  }
}
BENCHMARK(BM_BatchExecute_All)->Arg(2)->Arg(8)->Arg(32);

void BM_BatchExecute_AtLoop(benchmark::State& state) {
  const int n = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  std::vector<std::unique_ptr<ExecNode>> nodes;
  nodes.reserve(n);
  for (int i = 0; i < n; ++i) {
    nodes.emplace_back(MakeLinearAddChain(8));
  }
  ExecEngine engine;
  auto cs = engine.BatchCompile(nodes, reg);
  if (!cs.ok()) {
    state.SkipWithError("batch compile failed");
    return;
  }
  ExecContext ctx(4096);
  for (auto _ : state) {
    ctx.Clear();
    for (int i = 0; i < n; ++i) {
      RetType result;
      auto st = engine.ExecuteAt(static_cast<size_t>(i), ctx, nullptr, &result);
      benchmark::DoNotOptimize(result);
      if (!st.ok()) {
        state.SkipWithError("execute_at failed");
        break;
      }
    }
  }
}
BENCHMARK(BM_BatchExecute_AtLoop)->Arg(2)->Arg(8)->Arg(32);

// =============================================================================
// F. Native C++ baselines
// =============================================================================
//
// Hand-written C++ counterparts of selected B/C cases. They give a concrete
// "zero-overhead" reference point so the engine's numbers can be read as
// "JIT overhead over optimal native".
//
// NOTE: gh-pages plots the raw times, so do NOT rename these — the pairing
// with the engine variants (e.g. BM_Execute_Arith/0 vs BM_Native_Arith_i64)
// is implicit in the name. If you add a new engine variant, add its native
// twin right next to it.

void BM_Native_Arith_i64(benchmark::State& state) {
  int64_t a = 1234567;
  int64_t b = 7654321;
  for (auto _ : state) {
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    int64_t r = a * b;
    benchmark::DoNotOptimize(r);
  }
}
BENCHMARK(BM_Native_Arith_i64);

void BM_Native_Arith_f64(benchmark::State& state) {
  double a = 1.2345;
  double b = 6.789;
  for (auto _ : state) {
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    double r = a * b;
    benchmark::DoNotOptimize(r);
  }
}
BENCHMARK(BM_Native_Arith_f64);

void BM_Native_ListSum_i64(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  std::vector<int64_t> values;
  values.reserve(len);
  for (int i = 0; i < len; ++i) {
    values.push_back(i);
  }
  for (auto _ : state) {
    benchmark::DoNotOptimize(values.data());
    int64_t sum = std::accumulate(values.begin(), values.end(), int64_t{0});
    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(BM_Native_ListSum_i64)->Arg(16)->Arg(256)->Arg(4096);

}  // namespace

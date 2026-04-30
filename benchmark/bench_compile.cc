// JitFusion engine benchmarks.
//
// Benchmark names, once merged, MUST NOT be renamed — the CI pipeline
// (benchmark-action/github-action-benchmark) keys its historical data on the
// name. Adding new cases is fine; renaming breaks the time series on gh-pages.
//
// This binary is produced from multiple translation units sharing the
// helpers in bench_common.h; every TU registers its own BENCHMARK(...)
// entries into the single benchmark::internal registry and the linker
// folds them together. BENCHMARK_MAIN() must appear in exactly one TU —
// we put it at the bottom of this file.
//
// Coverage split across files:
//   A. Compile     -> bench_compile.cc        (this file)
//   B. Execute     -> bench_execute_core.cc
//   C. List        -> bench_list_agg.cc
//   G. List basic  -> bench_list_basic.cc
//   H. Elem unary  -> bench_list_basic.cc
//   I. Elem binary -> bench_list_binary.cc
//   J. Compare     -> bench_list_compare.cc
//   D/E/F          -> bench_native_base.cc

#include "benchmark/bench_common.h"

namespace {

using ::jitfusion::bench::CompileOrDie;
using ::jitfusion::bench::MakeDeepNestedIf;
using ::jitfusion::bench::MakeLinearAddChain;
using ::jitfusion::bench::MakeListSumExpr;
using ::jitfusion::bench::MakeManyAbsCalls;
using ::jitfusion::bench::MakeRegistry;
using ::jitfusion::BinaryOPNode;
using ::jitfusion::BinaryOPType;
using ::jitfusion::ConstantValueNode;
using ::jitfusion::ExecEngine;
using ::jitfusion::ExecEngineOption;
using ::jitfusion::ExecNode;

// =============================================================================
// A. Compile
// =============================================================================

// A-1: Baseline compile cost for the smallest possible expression. Reflects
// the fixed LLJIT spin-up overhead.
void BM_Compile_MinimalExpr(benchmark::State& state) {
  auto reg = MakeRegistry();
  for (auto _ : state) {
    std::unique_ptr<ExecNode> lhs(new ConstantValueNode(static_cast<int32_t>(1)));
    std::unique_ptr<ExecNode> rhs(new ConstantValueNode(static_cast<int32_t>(2)));
    std::unique_ptr<ExecNode> node(new BinaryOPNode(BinaryOPType::kAdd, std::move(lhs), std::move(rhs)));
    ExecEngine engine;
    auto st = engine.Compile(node, reg);
    benchmark::DoNotOptimize(st);
    if (!st.ok()) {
      state.SkipWithError("compile failed");
    }
  }
}
BENCHMARK(BM_Compile_MinimalExpr)->Unit(benchmark::kMillisecond);

// A-2: Linear arithmetic chain — AST size vs compile time.
void BM_Compile_LinearChain(benchmark::State& state) {
  const int depth = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  for (auto _ : state) {
    auto node = MakeLinearAddChain(depth);
    ExecEngine engine;
    auto st = engine.Compile(node, reg);
    benchmark::DoNotOptimize(st);
    if (!st.ok()) {
      state.SkipWithError("compile failed");
    }
  }
}
BENCHMARK(BM_Compile_LinearChain)->Arg(10)->Arg(100)->Arg(1000)->Unit(benchmark::kMillisecond);

// A-3: Deeply nested if — control-flow complexity vs compile time.
void BM_Compile_DeepNested(benchmark::State& state) {
  const int depth = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  for (auto _ : state) {
    auto node = MakeDeepNestedIf(depth);
    ExecEngine engine;
    auto st = engine.Compile(node, reg);
    benchmark::DoNotOptimize(st);
    if (!st.ok()) {
      state.SkipWithError("compile failed");
    }
  }
}
BENCHMARK(BM_Compile_DeepNested)->Arg(10)->Arg(50)->Arg(200)->Unit(benchmark::kMillisecond);

// A-4: Many builtin function calls — exercises registry lookup + call-gen.
void BM_Compile_ManyFunctionCalls(benchmark::State& state) {
  const int depth = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  for (auto _ : state) {
    auto node = MakeManyAbsCalls(depth);
    ExecEngine engine;
    auto st = engine.Compile(node, reg);
    benchmark::DoNotOptimize(st);
    if (!st.ok()) {
      state.SkipWithError("compile failed");
    }
  }
}
BENCHMARK(BM_Compile_ManyFunctionCalls)->Arg(10)->Arg(100)->Unit(benchmark::kMillisecond);

// A-5: Compile cost of a list aggregation expression.
void BM_Compile_ListOps(benchmark::State& state) {
  auto reg = MakeRegistry();
  for (auto _ : state) {
    auto node = MakeListSumExpr(64);
    ExecEngine engine;
    auto st = engine.Compile(node, reg);
    benchmark::DoNotOptimize(st);
    if (!st.ok()) {
      state.SkipWithError("compile failed");
    }
  }
}
BENCHMARK(BM_Compile_ListOps)->Unit(benchmark::kMillisecond);

// A-6: Batch compile vs. N independent Compile() calls — measures how well the
// JIT setup amortises across multiple ASTs sharing one module.
void BM_BatchCompile(benchmark::State& state) {
  const int n = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  for (auto _ : state) {
    std::vector<std::unique_ptr<ExecNode>> nodes;
    nodes.reserve(n);
    for (int i = 0; i < n; ++i) {
      nodes.emplace_back(MakeLinearAddChain(8));
    }
    ExecEngine engine;
    auto st = engine.BatchCompile(nodes, reg);
    benchmark::DoNotOptimize(st);
    if (!st.ok()) {
      state.SkipWithError("batch compile failed");
    }
  }
}
BENCHMARK(BM_BatchCompile)->Arg(2)->Arg(8)->Arg(32)->Unit(benchmark::kMillisecond);

// A-7: dump_ir toggle — how much does IR-text capture cost at compile time?
void BM_Compile_DumpIrOn(benchmark::State& state) {
  auto reg = MakeRegistry();
  for (auto _ : state) {
    auto node = MakeLinearAddChain(32);
    ExecEngineOption opt;
    opt.dump_ir = true;
    ExecEngine engine(opt);
    auto st = engine.Compile(node, reg);
    benchmark::DoNotOptimize(st);
    if (!st.ok()) {
      state.SkipWithError("compile failed");
    }
  }
}
BENCHMARK(BM_Compile_DumpIrOn)->Unit(benchmark::kMillisecond);

void BM_Compile_DumpIrOff(benchmark::State& state) {
  auto reg = MakeRegistry();
  for (auto _ : state) {
    auto node = MakeLinearAddChain(32);
    ExecEngineOption opt;
    opt.dump_ir = false;
    ExecEngine engine(opt);
    auto st = engine.Compile(node, reg);
    benchmark::DoNotOptimize(st);
    if (!st.ok()) {
      state.SkipWithError("compile failed");
    }
  }
}
BENCHMARK(BM_Compile_DumpIrOff)->Unit(benchmark::kMillisecond);

}  // namespace

BENCHMARK_MAIN();

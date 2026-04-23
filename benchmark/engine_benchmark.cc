// JitFusion engine benchmarks.
//
// Benchmark names, once merged, MUST NOT be renamed — the CI pipeline
// (benchmark-action/github-action-benchmark) keys its historical data on the
// name. Adding new cases is fine; renaming breaks the time series on gh-pages.
//
// Coverage:
//   A. Compile     : MinimalExpr / LinearChain / DeepNested / ManyFunctionCalls
//                    / ListOps / BatchCompile / DumpIr on-off
//   B. Execute     : ConstFold / Arith (parameterised per type) / LongChain
//                    / IfThenElse / Switch / IfBlock / RefNode / FunctionCall
//   C. List        : full aggregation surface — Sum / Max / Min / Avg / Median
//                    / SortedMedian / CountDistinct / CountBits / GetAt
//   G. List basic  : Len / Unique / SortAsc / SortDesc / Truncate /
//                    CastStringList / MurmurHash3X8632 / ListConcat / in
//   H. Elem unary  : ListExp / Log / Log2 / Log10 / Ceil / Floor / Round
//   I. Elem binary : ListAdd / Sub / Mul / Div / Mod / Max / Min /
//                    BitwiseAnd / Or / Xor, each in Scalar and list-list forms
//   J. Compare     : Gen{Equal,NotEqual,Large,LargeEqual,Less,LessEqual}Bitmap
//                    / If{Equal,NotEqual,Large,LargeEqual,Less,LessEqual}
//                    / IfByBitmap / FilterByBitmap
//   D. ExecContext : arena reuse vs. fresh-per-call
//   E. Batch       : ExecuteAll / ExecuteAt loop after BatchCompile
//   F. Native base : hand-written C++ baselines to calibrate JIT overhead

#include <benchmark/benchmark.h>

#include <cstdint>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "exec_engine.h"
#include "exec_node.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace {

using jitfusion::BinaryOPNode;
using jitfusion::BinaryOPType;
using jitfusion::ConstantListValueNode;
using jitfusion::ConstantValueNode;
using jitfusion::ExecContext;
using jitfusion::ExecEngine;
using jitfusion::ExecEngineOption;
using jitfusion::ExecNode;
using jitfusion::FunctionNode;
using jitfusion::FunctionRegistry;
using jitfusion::FunctionRegistryFactory;
using jitfusion::FunctionSignature;
using jitfusion::IfBlockNode;
using jitfusion::IfNode;
using jitfusion::NoOPNode;
using jitfusion::OutputNode;
using jitfusion::RefNode;
using jitfusion::RetType;
using jitfusion::SwitchNode;
using jitfusion::ValueType;

// -----------------------------------------------------------------------------
// AST construction helpers
// -----------------------------------------------------------------------------

// Linear chain: ((((c0 + c1) + c2) + c3) + ...)
std::unique_ptr<ExecNode> MakeLinearAddChain(int depth) {
  std::unique_ptr<ExecNode> cur(new ConstantValueNode(static_cast<int64_t>(0)));
  for (int i = 1; i <= depth; ++i) {
    std::unique_ptr<ExecNode> rhs(new ConstantValueNode(static_cast<int64_t>(i)));
    cur = std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kAdd, std::move(cur), std::move(rhs)));
  }
  return cur;
}

// Deeply nested if-else chain: if(c,1,if(c,2,if(c,3,...)))
std::unique_ptr<ExecNode> MakeDeepNestedIf(int depth) {
  std::unique_ptr<ExecNode> cur(new ConstantValueNode(static_cast<int32_t>(0)));
  for (int i = depth; i >= 1; --i) {
    std::unique_ptr<ExecNode> cond(new ConstantValueNode(static_cast<uint8_t>(i & 1)));
    std::unique_ptr<ExecNode> then_v(new ConstantValueNode(static_cast<int32_t>(i)));
    std::vector<std::unique_ptr<ExecNode>> args;
    args.emplace_back(std::move(cond));
    args.emplace_back(std::move(then_v));
    args.emplace_back(std::move(cur));
    cur = std::unique_ptr<ExecNode>(new IfNode(std::move(args)));
  }
  return cur;
}

// abs(abs(abs(...abs(c))))
std::unique_ptr<ExecNode> MakeManyAbsCalls(int depth) {
  std::unique_ptr<ExecNode> cur(new ConstantValueNode(static_cast<int32_t>(-42)));
  for (int i = 0; i < depth; ++i) {
    std::vector<std::unique_ptr<ExecNode>> args;
    args.emplace_back(std::move(cur));
    cur = std::unique_ptr<ExecNode>(new FunctionNode("abs", std::move(args)));
  }
  return cur;
}

// Build `func_name(const_list_of_length(list_len))`.
std::unique_ptr<ExecNode> MakeListUnaryCall(const std::string& func_name, int list_len) {
  std::vector<int64_t> values;
  values.reserve(list_len);
  for (int i = 0; i < list_len; ++i) {
    values.push_back(i);
  }
  std::unique_ptr<ExecNode> list_node(new ConstantListValueNode(std::move(values)));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(list_node));
  return std::unique_ptr<ExecNode>(new FunctionNode(func_name, std::move(args)));
}

// Convenience alias kept for readability.
std::unique_ptr<ExecNode> MakeListSumExpr(int list_len) { return MakeListUnaryCall("Sum", list_len); }

// Build `func_name(list, exec_ctx)` — one-arg list kernels that require an
// ExecContext pointer (arithmetic/basic kernels that allocate output lists).
std::unique_ptr<ExecNode> MakeListUnaryCtxCall(const std::string& func_name, int list_len) {
  std::vector<int64_t> values;
  values.reserve(list_len);
  for (int i = 0; i < list_len; ++i) {
    values.push_back(i + 1);  // avoid zero — log*/div-like kernels are sensitive to it.
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(values)));
  args.emplace_back(new jitfusion::ExecContextNode());
  return std::unique_ptr<ExecNode>(new FunctionNode(func_name, std::move(args)));
}

// Build `func_name(list<f64>, exec_ctx)` — unary kernels that only accept float lists
// (ListCeil, ListFloor, ListRound).
std::unique_ptr<ExecNode> MakeListUnaryCtxCallF64(const std::string& func_name, int list_len) {
  std::vector<double> values;
  values.reserve(list_len);
  for (int i = 0; i < list_len; ++i) {
    values.push_back(static_cast<double>(i) + 0.5);
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(values)));
  args.emplace_back(new jitfusion::ExecContextNode());
  return std::unique_ptr<ExecNode>(new FunctionNode(func_name, std::move(args)));
}

// Build `func_name(list<int64>, scalar_int, exec_ctx)` — list-scalar broadcast.
std::unique_ptr<ExecNode> MakeListScalarCtxCall(const std::string& func_name, int list_len, int64_t scalar_val) {
  std::vector<int64_t> values;
  values.reserve(list_len);
  for (int i = 0; i < list_len; ++i) {
    values.push_back(i + 1);
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(values)));
  args.emplace_back(new ConstantValueNode(scalar_val));
  args.emplace_back(new jitfusion::ExecContextNode());
  return std::unique_ptr<ExecNode>(new FunctionNode(func_name, std::move(args)));
}

// Build `func_name(list<uint64>, scalar<uint64>, exec_ctx)` — bitwise list-scalar.
// Bitwise kernels are only registered for unsigned integer types.
std::unique_ptr<ExecNode> MakeListScalarCtxCallU64(const std::string& func_name, int list_len, uint64_t scalar_val) {
  std::vector<uint64_t> values;
  values.reserve(list_len);
  for (int i = 0; i < list_len; ++i) {
    values.push_back(static_cast<uint64_t>(i + 1));
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(values)));
  args.emplace_back(new ConstantValueNode(scalar_val));
  args.emplace_back(new jitfusion::ExecContextNode());
  return std::unique_ptr<ExecNode>(new FunctionNode(func_name, std::move(args)));
}

// Build `func_name(list<uint64>, list<uint64>, exec_ctx)` — bitwise list-list.
std::unique_ptr<ExecNode> MakeListListCtxCallU64(const std::string& func_name, int list_len) {
  std::vector<uint64_t> lhs;
  std::vector<uint64_t> rhs;
  lhs.reserve(list_len);
  rhs.reserve(list_len);
  for (int i = 0; i < list_len; ++i) {
    lhs.push_back(static_cast<uint64_t>(i + 1));
    rhs.push_back(static_cast<uint64_t>(list_len - i));
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(lhs)));
  args.emplace_back(new ConstantListValueNode(std::move(rhs)));
  args.emplace_back(new jitfusion::ExecContextNode());
  return std::unique_ptr<ExecNode>(new FunctionNode(func_name, std::move(args)));
}

// Build `func_name(list<int64>, list<int64>, exec_ctx)` — element-wise list-list.
std::unique_ptr<ExecNode> MakeListListCtxCall(const std::string& func_name, int list_len) {
  std::vector<int64_t> lhs;
  std::vector<int64_t> rhs;
  lhs.reserve(list_len);
  rhs.reserve(list_len);
  for (int i = 0; i < list_len; ++i) {
    lhs.push_back(i + 1);
    rhs.push_back(list_len - i);
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(lhs)));
  args.emplace_back(new ConstantListValueNode(std::move(rhs)));
  args.emplace_back(new jitfusion::ExecContextNode());
  return std::unique_ptr<ExecNode>(new FunctionNode(func_name, std::move(args)));
}

// Build `func_name(list<int64>, scalar, scalar, exec_ctx)` — the IfEqual /
// IfLess / etc. family: "for each element, if cond(elem, pivot) pick a else b".
std::unique_ptr<ExecNode> MakeListIfSelectCall(const std::string& func_name, int list_len) {
  std::vector<int64_t> values;
  values.reserve(list_len);
  for (int i = 0; i < list_len; ++i) {
    values.push_back(i + 1);
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(values)));
  args.emplace_back(new ConstantValueNode(static_cast<int64_t>(list_len / 2)));
  args.emplace_back(new ConstantValueNode(static_cast<int64_t>(-1)));
  args.emplace_back(new jitfusion::ExecContextNode());
  return std::unique_ptr<ExecNode>(new FunctionNode(func_name, std::move(args)));
}

std::unique_ptr<FunctionRegistry> MakeRegistry() {
  std::unique_ptr<FunctionRegistry> r;
  auto st = FunctionRegistryFactory::CreateFunctionRegistry(&r);
  if (!st.ok()) {
    std::abort();
  }
  return r;
}

// Side-effect sink used by benchmarks whose root otherwise would be dead-code
// eliminated (NoOPNode / IfBlockNode return void, so without a side effect
// LLVM's DCE collapses the whole entry function to `ret void` and the
// measurement reflects only the empty-JIT-call overhead, not the AST node
// under test).
void StoreI32(void* output, int32_t index, int32_t value) { reinterpret_cast<int32_t*>(output)[index] = value; }

std::unique_ptr<FunctionRegistry> MakeRegistryWithStore() {
  auto r = MakeRegistry();
  FunctionSignature sig("store", {ValueType::kPtr, ValueType::kI32, ValueType::kI32}, ValueType::kVoid);
  auto st = r->RegisterStoreCFunc(sig, reinterpret_cast<void*>(StoreI32), 1);
  if (!st.ok()) {
    std::abort();
  }
  return r;
}

// Build `store(output, index, value)` as a leaf node. Consumes the value so
// DCE can't remove it.
std::unique_ptr<ExecNode> MakeStoreI32(int32_t index, std::unique_ptr<ExecNode> value) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new OutputNode());
  args.emplace_back(new ConstantValueNode(index));
  args.emplace_back(std::move(value));
  return std::unique_ptr<ExecNode>(new FunctionNode("store", std::move(args)));
}

// Compile one AST or abort. Keeps benchmark code free of ASSERT noise.
std::unique_ptr<ExecEngine> CompileOrDie(std::unique_ptr<ExecNode> node, const std::unique_ptr<FunctionRegistry>& reg,
                                         bool dump_ir = false) {
  ExecEngineOption opt;
  opt.dump_ir = dump_ir;
  auto engine = std::make_unique<ExecEngine>(opt);
  std::unique_ptr<ExecNode> holder = std::move(node);
  auto st = engine->Compile(holder, reg);
  if (!st.ok()) {
    std::abort();
  }
  return engine;
}

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

// =============================================================================
// B. Execute
// =============================================================================

// B-1: Post-constant-fold execute — lower bound on JIT call overhead.
void BM_Execute_ConstFold(benchmark::State& state) {
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
BENCHMARK(BM_Execute_ConstFold);

// B-2: Scalar arithmetic, representative types (i64 / f64).
// Arg(0): 0 = i64, 1 = f64
void BM_Execute_Arith(benchmark::State& state) {
  const int kind = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();

  std::unique_ptr<ExecNode> node;
  if (kind == 0) {
    std::unique_ptr<ExecNode> l(new ConstantValueNode(static_cast<int64_t>(1234567)));
    std::unique_ptr<ExecNode> r(new ConstantValueNode(static_cast<int64_t>(7654321)));
    node = std::make_unique<BinaryOPNode>(BinaryOPType::kMul, std::move(l), std::move(r));
  } else {
    std::unique_ptr<ExecNode> l(new ConstantValueNode(1.2345));
    std::unique_ptr<ExecNode> r(new ConstantValueNode(6.789));
    node = std::make_unique<BinaryOPNode>(BinaryOPType::kMul, std::move(l), std::move(r));
  }
  auto engine = CompileOrDie(std::move(node), reg);
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
BENCHMARK(BM_Execute_Arith)->Arg(0)->Arg(1);

// B-3: Execute cost of a long arithmetic chain — should stay flat w.r.t. AST
// size once the JIT has folded constants.
void BM_Execute_LongChain(benchmark::State& state) {
  const int depth = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeLinearAddChain(depth), reg);
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
BENCHMARK(BM_Execute_LongChain)->Arg(10)->Arg(100)->Arg(1000);

// B-4: Single-level if.
void BM_Execute_IfThenElse(benchmark::State& state) {
  auto reg = MakeRegistry();
  std::unique_ptr<ExecNode> cond(new ConstantValueNode(static_cast<uint8_t>(1)));
  std::unique_ptr<ExecNode> t(new ConstantValueNode(static_cast<int32_t>(42)));
  std::unique_ptr<ExecNode> f(new ConstantValueNode(static_cast<int32_t>(-42)));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(cond));
  args.emplace_back(std::move(t));
  args.emplace_back(std::move(f));
  std::unique_ptr<ExecNode> node(new IfNode(std::move(args)));

  auto engine = CompileOrDie(std::move(node), reg);
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
BENCHMARK(BM_Execute_IfThenElse);

// B-5: N-way switch. All conditions are 0 except the last, so evaluation
// traverses every branch (worst-case path).
void BM_Execute_Switch(benchmark::State& state) {
  const int n = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  std::vector<std::unique_ptr<ExecNode>> args;
  for (int i = 0; i < n; ++i) {
    uint8_t cond_val = (i == n - 1) ? 1 : 0;
    args.emplace_back(new ConstantValueNode(cond_val));
    args.emplace_back(new ConstantValueNode(static_cast<int32_t>(i)));
  }
  args.emplace_back(new ConstantValueNode(static_cast<int32_t>(-1)));  // default
  std::unique_ptr<ExecNode> node(new SwitchNode(std::move(args)));

  auto engine = CompileOrDie(std::move(node), reg);
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
BENCHMARK(BM_Execute_Switch)->Arg(2)->Arg(8)->Arg(32);

// B-6: IfBlock — multi-statement blocks.
// Args layout: [cond1, block1, cond2, block2, ..., else_block].
// Each block binds a named `result` variable; after the IfBlock we
// store(ref("result")) so the branches are observably live. Without this
// sink LLVM would collapse the whole function to `ret void`.
void BM_Execute_IfBlock(benchmark::State& state) {
  auto reg = MakeRegistryWithStore();

  auto make_block = [](int32_t v) {
    auto block = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
    static_cast<NoOPNode*>(block.get())->AppendArgs("result", std::unique_ptr<ExecNode>(new ConstantValueNode(v)));
    return block;
  };

  std::vector<std::unique_ptr<ExecNode>> if_args;
  if_args.emplace_back(new ConstantValueNode(static_cast<uint8_t>(1)));
  if_args.emplace_back(make_block(100));
  if_args.emplace_back(new ConstantValueNode(static_cast<uint8_t>(0)));
  if_args.emplace_back(make_block(200));
  if_args.emplace_back(make_block(300));  // else block
  auto if_block = std::unique_ptr<ExecNode>(new IfBlockNode(std::move(if_args)));

  // root = [ result = 0, <if_block assigns result>, store(output, 0, ref("result")) ]
  // `result` must be declared in the outer scope so RefNode("result") can
  // resolve it after the IfBlock; this matches the pattern used in
  // test/if_block_test.cc.
  auto root = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(root.get())
      ->AppendArgs("result", std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(0))));
  static_cast<NoOPNode*>(root.get())->AppendArgs(std::move(if_block));
  static_cast<NoOPNode*>(root.get())->AppendArgs(MakeStoreI32(0, std::unique_ptr<ExecNode>(new RefNode("result"))));

  auto engine = CompileOrDie(std::move(root), reg);
  ExecContext ctx(4096);
  int32_t output = 0;
  for (auto _ : state) {
    ctx.Clear();
    auto st = engine->Execute(ctx, nullptr, static_cast<void*>(&output));
    benchmark::DoNotOptimize(output);
    if (!st.ok()) {
      state.SkipWithError("execute failed");
    }
  }
}
BENCHMARK(BM_Execute_IfBlock);

// B-7: RefNode — common sub-expression reuse.
// Names a sub-expression via the NoOPNode named-form and references it
// through RefNode. The result of the multiplication is written out via
// `store(output, 0, ...)`; without that side effect LLVM's DCE would
// collapse the whole entry function to `ret void` because NoOPNode itself
// returns void.
void BM_Execute_RefNode(benchmark::State& state) {
  auto reg = MakeRegistryWithStore();

  // x = 10 + 20
  std::unique_ptr<ExecNode> inner(
      new BinaryOPNode(BinaryOPType::kAdd, std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(10))),
                       std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(20)))));

  // ref("x") * ref("x")
  auto product = std::unique_ptr<ExecNode>(new BinaryOPNode(
      BinaryOPType::kMul, std::unique_ptr<ExecNode>(new RefNode("x")), std::unique_ptr<ExecNode>(new RefNode("x"))));

  std::vector<std::string> names = {"x", ""};
  std::vector<std::unique_ptr<ExecNode>> items;
  items.emplace_back(std::move(inner));
  items.emplace_back(MakeStoreI32(0, std::move(product)));
  std::unique_ptr<ExecNode> node(new NoOPNode(std::move(names), std::move(items)));

  auto engine = CompileOrDie(std::move(node), reg);
  ExecContext ctx(4096);
  int32_t output = 0;
  for (auto _ : state) {
    ctx.Clear();
    auto st = engine->Execute(ctx, nullptr, static_cast<void*>(&output));
    benchmark::DoNotOptimize(output);
    if (!st.ok()) {
      state.SkipWithError("execute failed");
    }
  }
}
BENCHMARK(BM_Execute_RefNode);

// B-8: Builtin function-call execute cost (abs).
void BM_Execute_FunctionCall_Builtin(benchmark::State& state) {
  auto reg = MakeRegistry();
  std::unique_ptr<ExecNode> arg(new ConstantValueNode(static_cast<int32_t>(-12345)));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(arg));
  std::unique_ptr<ExecNode> node(new FunctionNode("abs", std::move(args)));

  auto engine = CompileOrDie(std::move(node), reg);
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
BENCHMARK(BM_Execute_FunctionCall_Builtin);

// =============================================================================
// C. List aggregation — kernel cost vs. list length
// =============================================================================
//
// These cases share a single shape (aggregate kernel called on a constant
// int64 list) and vary only the length. Names and argument lists are fixed
// because gh-pages indexes by the full name incl. "/N".

void BM_Execute_ListSum(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListUnaryCall("Sum", len), reg);
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
BENCHMARK(BM_Execute_ListSum)->Arg(16)->Arg(256)->Arg(4096);

void BM_Execute_ListMax(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListUnaryCall("Max", len), reg);
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
BENCHMARK(BM_Execute_ListMax)->Arg(16)->Arg(256)->Arg(4096);

void BM_Execute_ListAvg(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListUnaryCall("Avg", len), reg);
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
BENCHMARK(BM_Execute_ListAvg)->Arg(16)->Arg(256)->Arg(4096);

// -----------------------------------------------------------------------------
// C-extra: remaining aggregation kernels.
// Every kernel gets its own benchmark name so gh-pages can plot it
// independently. Two list lengths (256, 4096) are enough to distinguish
// O(N) kernels from O(N log N) / O(N^2) ones.
// -----------------------------------------------------------------------------

void BM_Execute_ListMin(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListUnaryCall("Min", len), reg);
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
BENCHMARK(BM_Execute_ListMin)->Arg(256)->Arg(4096);

void BM_Execute_ListMedian(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListUnaryCall("Median", len), reg);
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
BENCHMARK(BM_Execute_ListMedian)->Arg(256)->Arg(4096);

void BM_Execute_ListSortedMedian(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListUnaryCall("SortedMedian", len), reg);
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
BENCHMARK(BM_Execute_ListSortedMedian)->Arg(256)->Arg(4096);

void BM_Execute_ListCountDistinct(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListUnaryCall("CountDistinct", len), reg);
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
BENCHMARK(BM_Execute_ListCountDistinct)->Arg(256)->Arg(4096);

// CountBits is only registered for U8List.
void BM_Execute_ListCountBits(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  std::vector<uint8_t> values(len, static_cast<uint8_t>(0xA5));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(values)));
  std::unique_ptr<ExecNode> node(new FunctionNode("CountBits", std::move(args)));

  auto engine = CompileOrDie(std::move(node), reg);
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
BENCHMARK(BM_Execute_ListCountBits)->Arg(256)->Arg(4096);

// GetAt(list, 0) — random access cost, should be O(1).
void BM_Execute_ListGetAt(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  std::vector<int64_t> values;
  values.reserve(len);
  for (int i = 0; i < len; ++i) {
    values.push_back(i);
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(values)));
  args.emplace_back(new ConstantValueNode(static_cast<uint32_t>(0)));
  std::unique_ptr<ExecNode> node(new FunctionNode("GetAt", std::move(args)));

  auto engine = CompileOrDie(std::move(node), reg);
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
BENCHMARK(BM_Execute_ListGetAt)->Arg(256)->Arg(4096);

// =============================================================================
// G. List basic — shape-manipulation kernels
// =============================================================================

// Len is O(1); one length sample is enough.
void BM_Execute_ListLen(benchmark::State& state) {
  auto reg = MakeRegistry();
  std::vector<int64_t> values(256, 0);
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(values)));
  std::unique_ptr<ExecNode> node(new FunctionNode("Len", std::move(args)));

  auto engine = CompileOrDie(std::move(node), reg);
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
BENCHMARK(BM_Execute_ListLen);

void BM_Execute_ListUnique(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListUnaryCtxCall("Unique", len), reg);
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
BENCHMARK(BM_Execute_ListUnique)->Arg(256)->Arg(4096);

void BM_Execute_ListSortAsc(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListUnaryCtxCall("SortAsc", len), reg);
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
BENCHMARK(BM_Execute_ListSortAsc)->Arg(256)->Arg(4096);

void BM_Execute_ListSortDesc(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListUnaryCtxCall("SortDesc", len), reg);
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
BENCHMARK(BM_Execute_ListSortDesc)->Arg(256)->Arg(4096);

// Truncate takes (list, u32) — keep the first `keep_n` elements.
void BM_Execute_ListTruncate(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  std::vector<int64_t> values;
  values.reserve(len);
  for (int i = 0; i < len; ++i) {
    values.push_back(i);
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(values)));
  args.emplace_back(new ConstantValueNode(static_cast<uint32_t>(len / 2)));
  std::unique_ptr<ExecNode> node(new FunctionNode("Truncate", std::move(args)));

  auto engine = CompileOrDie(std::move(node), reg);
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
BENCHMARK(BM_Execute_ListTruncate)->Arg(256)->Arg(4096);

void BM_Execute_ListCastStringList(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListUnaryCtxCall("CastStringList", len), reg);
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
BENCHMARK(BM_Execute_ListCastStringList)->Arg(256)->Arg(4096);

void BM_Execute_ListMurmurHash3X8632(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListUnaryCall("MurmurHash3X8632", len), reg);
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
BENCHMARK(BM_Execute_ListMurmurHash3X8632)->Arg(256)->Arg(4096);

// ListConcat(listA, listB, ctx).
void BM_Execute_ListConcat(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeListListCtxCall("ListConcat", len), reg);
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
BENCHMARK(BM_Execute_ListConcat)->Arg(256)->Arg(4096);

// `in(needle, haystack_list)` — scalar-in-list membership test (not list-out).
void BM_Execute_ListIn(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  std::vector<int64_t> values;
  values.reserve(len);
  for (int i = 0; i < len; ++i) {
    values.push_back(i);
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  // Search for a value near the end to force worst-case linear scan.
  args.emplace_back(new ConstantValueNode(static_cast<int64_t>(len - 1)));
  args.emplace_back(new ConstantListValueNode(std::move(values)));
  std::unique_ptr<ExecNode> node(new FunctionNode("in", std::move(args)));

  auto engine = CompileOrDie(std::move(node), reg);
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
BENCHMARK(BM_Execute_ListIn)->Arg(256)->Arg(4096);

// =============================================================================
// H. List element-wise unary (func(list, ctx) -> list)
// =============================================================================

#define JITFUSION_DEFINE_LIST_UNARY_BM(NAME)                           \
  void BM_Execute_##NAME(benchmark::State& state) {                    \
    const int len = static_cast<int>(state.range(0));                  \
    auto reg = MakeRegistry();                                         \
    auto engine = CompileOrDie(MakeListUnaryCtxCall(#NAME, len), reg); \
    ExecContext ctx(4096);                                             \
    for (auto _ : state) {                                             \
      ctx.Clear();                                                     \
      RetType result;                                                  \
      auto st = engine->Execute(ctx, nullptr, &result);                \
      benchmark::DoNotOptimize(result);                                \
      if (!st.ok()) {                                                  \
        state.SkipWithError("execute failed");                         \
      }                                                                \
    }                                                                  \
  }                                                                    \
  BENCHMARK(BM_Execute_##NAME)->Arg(256)->Arg(4096)  // NOLINT

JITFUSION_DEFINE_LIST_UNARY_BM(ListExp);
JITFUSION_DEFINE_LIST_UNARY_BM(ListLog);
JITFUSION_DEFINE_LIST_UNARY_BM(ListLog2);
JITFUSION_DEFINE_LIST_UNARY_BM(ListLog10);
#undef JITFUSION_DEFINE_LIST_UNARY_BM

// ListCeil / ListFloor / ListRound only accept float lists.
#define JITFUSION_DEFINE_LIST_UNARY_F64_BM(NAME)                          \
  void BM_Execute_##NAME(benchmark::State& state) {                       \
    const int len = static_cast<int>(state.range(0));                     \
    auto reg = MakeRegistry();                                            \
    auto engine = CompileOrDie(MakeListUnaryCtxCallF64(#NAME, len), reg); \
    ExecContext ctx(4096);                                                \
    for (auto _ : state) {                                                \
      ctx.Clear();                                                        \
      RetType result;                                                     \
      auto st = engine->Execute(ctx, nullptr, &result);                   \
      benchmark::DoNotOptimize(result);                                   \
      if (!st.ok()) {                                                     \
        state.SkipWithError("execute failed");                            \
      }                                                                   \
    }                                                                     \
  }                                                                       \
  BENCHMARK(BM_Execute_##NAME)->Arg(256)->Arg(4096)  // NOLINT

JITFUSION_DEFINE_LIST_UNARY_F64_BM(ListCeil);
JITFUSION_DEFINE_LIST_UNARY_F64_BM(ListFloor);
JITFUSION_DEFINE_LIST_UNARY_F64_BM(ListRound);

#undef JITFUSION_DEFINE_LIST_UNARY_F64_BM

// =============================================================================
// I. List element-wise binary — two shapes
// =============================================================================
//
// Every kernel in this family is overloaded for both:
//   - `Scalar` : list<T> OP scalar<T>  -> list<T>   (broadcast)
//   - `LL`     : list<T> OP list<T>    -> list<T>   (zip)
// Both shapes are exercised so we can see whether the list-list path takes
// a different code branch (e.g. lacks vectorisation).

#define JITFUSION_DEFINE_LIST_BINARY_BM(NAME)                              \
  void BM_Execute_##NAME##_Scalar(benchmark::State& state) {               \
    const int len = static_cast<int>(state.range(0));                      \
    auto reg = MakeRegistry();                                             \
    auto engine = CompileOrDie(MakeListScalarCtxCall(#NAME, len, 2), reg); \
    ExecContext ctx(4096);                                                 \
    for (auto _ : state) {                                                 \
      ctx.Clear();                                                         \
      RetType result;                                                      \
      auto st = engine->Execute(ctx, nullptr, &result);                    \
      benchmark::DoNotOptimize(result);                                    \
      if (!st.ok()) {                                                      \
        state.SkipWithError("execute failed");                             \
      }                                                                    \
    }                                                                      \
  }                                                                        \
  BENCHMARK(BM_Execute_##NAME##_Scalar)->Arg(4096);                        \
  void BM_Execute_##NAME##_LL(benchmark::State& state) {                   \
    const int len = static_cast<int>(state.range(0));                      \
    auto reg = MakeRegistry();                                             \
    auto engine = CompileOrDie(MakeListListCtxCall(#NAME, len), reg);      \
    ExecContext ctx(4096);                                                 \
    for (auto _ : state) {                                                 \
      ctx.Clear();                                                         \
      RetType result;                                                      \
      auto st = engine->Execute(ctx, nullptr, &result);                    \
      benchmark::DoNotOptimize(result);                                    \
      if (!st.ok()) {                                                      \
        state.SkipWithError("execute failed");                             \
      }                                                                    \
    }                                                                      \
  }                                                                        \
  BENCHMARK(BM_Execute_##NAME##_LL)->Arg(4096)  // NOLINT

JITFUSION_DEFINE_LIST_BINARY_BM(ListAdd);
JITFUSION_DEFINE_LIST_BINARY_BM(ListSub);
JITFUSION_DEFINE_LIST_BINARY_BM(ListMul);
JITFUSION_DEFINE_LIST_BINARY_BM(ListDiv);
JITFUSION_DEFINE_LIST_BINARY_BM(ListMod);
JITFUSION_DEFINE_LIST_BINARY_BM(ListMax);
JITFUSION_DEFINE_LIST_BINARY_BM(ListMin);
#undef JITFUSION_DEFINE_LIST_BINARY_BM

// Bitwise kernels are only registered for unsigned integer types.
#define JITFUSION_DEFINE_LIST_BITWISE_BM(NAME)                                      \
  void BM_Execute_##NAME##_Scalar(benchmark::State& state) {                        \
    const int len = static_cast<int>(state.range(0));                               \
    auto reg = MakeRegistry();                                                      \
    auto engine = CompileOrDie(MakeListScalarCtxCallU64(#NAME, len, 0xFFULL), reg); \
    ExecContext ctx(4096);                                                          \
    for (auto _ : state) {                                                          \
      ctx.Clear();                                                                  \
      RetType result;                                                               \
      auto st = engine->Execute(ctx, nullptr, &result);                             \
      benchmark::DoNotOptimize(result);                                             \
      if (!st.ok()) {                                                               \
        state.SkipWithError("execute failed");                                      \
      }                                                                             \
    }                                                                               \
  }                                                                                 \
  BENCHMARK(BM_Execute_##NAME##_Scalar)->Arg(4096);                                 \
  void BM_Execute_##NAME##_LL(benchmark::State& state) {                            \
    const int len = static_cast<int>(state.range(0));                               \
    auto reg = MakeRegistry();                                                      \
    auto engine = CompileOrDie(MakeListListCtxCallU64(#NAME, len), reg);            \
    ExecContext ctx(4096);                                                          \
    for (auto _ : state) {                                                          \
      ctx.Clear();                                                                  \
      RetType result;                                                               \
      auto st = engine->Execute(ctx, nullptr, &result);                             \
      benchmark::DoNotOptimize(result);                                             \
      if (!st.ok()) {                                                               \
        state.SkipWithError("execute failed");                                      \
      }                                                                             \
    }                                                                               \
  }                                                                                 \
  BENCHMARK(BM_Execute_##NAME##_LL)->Arg(4096)  // NOLINT

JITFUSION_DEFINE_LIST_BITWISE_BM(ListBitwiseAnd);
JITFUSION_DEFINE_LIST_BITWISE_BM(ListBitwiseOr);
JITFUSION_DEFINE_LIST_BITWISE_BM(ListBitwiseXor);

#undef JITFUSION_DEFINE_LIST_BITWISE_BM

// =============================================================================
// J. List comparison / bitmap kernels
// =============================================================================
//
// Two sub-families here:
//   - `Gen*Bitmap(list, pivot, ctx)` : produce a U8List bitmap.
//   - `If*(list, pivot, alt, ctx)`   : pick from `list` vs `alt` per element.
// Plus two helpers that consume a bitmap: `IfByBitmap`, `FilterByBitmap`.

#define JITFUSION_DEFINE_LIST_GENBITMAP_BM(NAME)                                 \
  void BM_Execute_##NAME(benchmark::State& state) {                              \
    const int len = static_cast<int>(state.range(0));                            \
    auto reg = MakeRegistry();                                                   \
    auto engine = CompileOrDie(MakeListScalarCtxCall(#NAME, len, len / 2), reg); \
    ExecContext ctx(4096);                                                       \
    for (auto _ : state) {                                                       \
      ctx.Clear();                                                               \
      RetType result;                                                            \
      auto st = engine->Execute(ctx, nullptr, &result);                          \
      benchmark::DoNotOptimize(result);                                          \
      if (!st.ok()) {                                                            \
        state.SkipWithError("execute failed");                                   \
      }                                                                          \
    }                                                                            \
  }                                                                              \
  BENCHMARK(BM_Execute_##NAME)->Arg(4096)  // NOLINT

JITFUSION_DEFINE_LIST_GENBITMAP_BM(GenEqualBitmap);
JITFUSION_DEFINE_LIST_GENBITMAP_BM(GenNotEqualBitmap);
JITFUSION_DEFINE_LIST_GENBITMAP_BM(GenLargeBitmap);
JITFUSION_DEFINE_LIST_GENBITMAP_BM(GenLargeEqualBitmap);
JITFUSION_DEFINE_LIST_GENBITMAP_BM(GenLessBitmap);
JITFUSION_DEFINE_LIST_GENBITMAP_BM(GenLessEqualBitmap);

#undef JITFUSION_DEFINE_LIST_GENBITMAP_BM

#define JITFUSION_DEFINE_LIST_IFSELECT_BM(NAME)                        \
  void BM_Execute_##NAME(benchmark::State& state) {                    \
    const int len = static_cast<int>(state.range(0));                  \
    auto reg = MakeRegistry();                                         \
    auto engine = CompileOrDie(MakeListIfSelectCall(#NAME, len), reg); \
    ExecContext ctx(4096);                                             \
    for (auto _ : state) {                                             \
      ctx.Clear();                                                     \
      RetType result;                                                  \
      auto st = engine->Execute(ctx, nullptr, &result);                \
      benchmark::DoNotOptimize(result);                                \
      if (!st.ok()) {                                                  \
        state.SkipWithError("execute failed");                         \
      }                                                                \
    }                                                                  \
  }                                                                    \
  BENCHMARK(BM_Execute_##NAME)->Arg(4096)  // NOLINT

JITFUSION_DEFINE_LIST_IFSELECT_BM(IfEqual);
JITFUSION_DEFINE_LIST_IFSELECT_BM(IfNotEqual);
JITFUSION_DEFINE_LIST_IFSELECT_BM(IfLarge);
JITFUSION_DEFINE_LIST_IFSELECT_BM(IfLargeEqual);
JITFUSION_DEFINE_LIST_IFSELECT_BM(IfLess);
JITFUSION_DEFINE_LIST_IFSELECT_BM(IfLessEqual);

#undef JITFUSION_DEFINE_LIST_IFSELECT_BM

// IfByBitmap(bitmap_list<u8>, value_list<i64>, alt_scalar<i64>, ctx).
// For each i: bitmap[i] ? value_list[i] : alt_scalar.
void BM_Execute_IfByBitmap(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  std::vector<uint8_t> bitmap(len);
  std::vector<int64_t> values(len);
  for (int i = 0; i < len; ++i) {
    bitmap[i] = static_cast<uint8_t>(i & 1);
    values[i] = i;
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(bitmap)));
  args.emplace_back(new ConstantListValueNode(std::move(values)));
  args.emplace_back(new ConstantValueNode(static_cast<int64_t>(-1)));
  args.emplace_back(new jitfusion::ExecContextNode());
  std::unique_ptr<ExecNode> node(new FunctionNode("IfByBitmap", std::move(args)));

  auto engine = CompileOrDie(std::move(node), reg);
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
BENCHMARK(BM_Execute_IfByBitmap)->Arg(4096);

// FilterByBitmap(value_list<i64>, bitmap_list<u8>, popcount<u32>, ctx).
// Returns value_list[i] for all i where bitmap[i] != 0; popcount tells the
// kernel how much to allocate up front (here: len / 2 since the bitmap
// alternates 0/1).
void BM_Execute_FilterByBitmap(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  std::vector<int64_t> values(len);
  std::vector<uint8_t> bitmap(len);
  uint32_t popcnt = 0;
  for (int i = 0; i < len; ++i) {
    values[i] = i;
    bitmap[i] = static_cast<uint8_t>(i & 1);
    popcnt += bitmap[i];
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(values)));
  args.emplace_back(new ConstantListValueNode(std::move(bitmap)));
  args.emplace_back(new ConstantValueNode(popcnt));
  args.emplace_back(new jitfusion::ExecContextNode());
  std::unique_ptr<ExecNode> node(new FunctionNode("FilterByBitmap", std::move(args)));

  auto engine = CompileOrDie(std::move(node), reg);
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
BENCHMARK(BM_Execute_FilterByBitmap)->Arg(4096);

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

BENCHMARK_MAIN();

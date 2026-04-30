// B. Execute — core (non-list) execution benchmarks. See bench_compile.cc
// for the split overview. BENCHMARK_MAIN lives in bench_compile.cc.

#include "benchmark/bench_common.h"

namespace {

using ::jitfusion::bench::CompileOrDie;
using ::jitfusion::bench::MakeLinearAddChain;
using ::jitfusion::bench::MakeRegistry;
using ::jitfusion::bench::MakeRegistryWithStore;
using ::jitfusion::bench::MakeStoreI32;
using ::jitfusion::BinaryOPNode;
using ::jitfusion::BinaryOPType;
using ::jitfusion::ConstantValueNode;
using ::jitfusion::ExecContext;
using ::jitfusion::ExecNode;
using ::jitfusion::FunctionNode;
using ::jitfusion::IfBlockNode;
using ::jitfusion::IfNode;
using ::jitfusion::NoOPNode;
using ::jitfusion::RefNode;
using ::jitfusion::RetType;
using ::jitfusion::SwitchNode;

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

}  // namespace

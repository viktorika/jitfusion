// C. List aggregation + C-group. See bench_compile.cc for the split overview.

#include "benchmark/bench_common.h"

namespace {

using ::jitfusion::bench::CompileOrDie;
using ::jitfusion::bench::MakeListUnaryCall;
using ::jitfusion::bench::MakeRegistry;
using ::jitfusion::ConstantListValueNode;
using ::jitfusion::ConstantValueNode;
using ::jitfusion::ExecContext;
using ::jitfusion::ExecNode;
using ::jitfusion::FunctionNode;
using ::jitfusion::RetType;

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

// -----------------------------------------------------------------------------
// C-group: group-aggregation kernels.
//
// These functions share a common shape: a keys/values list drives a scatter
// aggregation over `distinct` buckets. Cost depends on both the input length
// and the distinct-bucket count, so each case is parameterised on two
// dimensions via Args({len, distinct}).
//
// The synthetic keys list is [0, 1, ..., distinct-1, 0, 1, ...] repeated up
// to `len`, so the produced group ids are stable regardless of hash-map
// iteration order.
//
// Three profile points cover the meaningful regimes:
//   * {4096, 16}   — large batch, very few groups (cache-friendly scatter).
//   * {4096, 256}  — large batch, moderate group count.
//   * {4096, 4096} — every element is its own group (worst-case hash churn).
//
// Only the sugar (3-arg) form is benchmarked: that is what user code calls,
// and it implicitly exercises the internal GroupCount CSE node as well.
// -----------------------------------------------------------------------------

// Build a keys list with `len` int64 elements whose values cycle through
// [0, distinct). Good enough to drive every group kernel deterministically.
std::unique_ptr<ExecNode> MakeGroupKeysList(int len, int distinct) {
  std::vector<int64_t> values;
  values.reserve(len);
  for (int i = 0; i < len; ++i) {
    values.push_back(static_cast<int64_t>(i % distinct));
  }
  return std::unique_ptr<ExecNode>(new ConstantListValueNode(std::move(values)));
}

// Build `GroupIndex(keys_list, exec_ctx)` — the only non-sugar group node in
// the family; its kernel is the 2-arg form.
std::unique_ptr<ExecNode> MakeGroupIndexNode(int len, int distinct) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(MakeGroupKeysList(len, distinct));
  args.emplace_back(new jitfusion::ExecContextNode());
  return std::unique_ptr<ExecNode>(new FunctionNode("GroupIndex", std::move(args)));
}

// C-group-1: GroupIndex standalone — hash-map construction cost.
void BM_Execute_GroupIndex(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  const int distinct = static_cast<int>(state.range(1));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeGroupIndexNode(len, distinct), reg);
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
BENCHMARK(BM_Execute_GroupIndex)->Args({4096, 16})->Args({4096, 256})->Args({4096, 4096});

// C-group-2: GroupCount(GroupIndex(keys)) — the max-scan reduction.
// This isolates the CSE target shared by every sugar in the family.
void BM_Execute_GroupCount(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  const int distinct = static_cast<int>(state.range(1));
  auto reg = MakeRegistry();
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(MakeGroupIndexNode(len, distinct));
  std::unique_ptr<ExecNode> node(new FunctionNode("GroupCount", std::move(args)));
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
BENCHMARK(BM_Execute_GroupCount)->Args({4096, 16})->Args({4096, 256})->Args({4096, 4096});

// Helper: build the sugar-form call `func_name(keys_or_values, GroupIndex(...), exec_ctx)`.
// The keys list drives GroupIndex; the same list is also fed as values/keys
// to the aggregate under test. That's fine because all aggregates accept
// integer lists and we measure compute time, not numerical content.
std::unique_ptr<ExecNode> MakeGroupSugarCall(const std::string& func_name, int len, int distinct) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(MakeGroupKeysList(len, distinct));
  args.emplace_back(MakeGroupIndexNode(len, distinct));
  args.emplace_back(new jitfusion::ExecContextNode());
  return std::unique_ptr<ExecNode>(new FunctionNode(func_name, std::move(args)));
}

// C-group-3: GroupKeys sugar — keys + group_index → per-bucket first-seen key.
void BM_Execute_GroupKeys(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  const int distinct = static_cast<int>(state.range(1));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeGroupSugarCall("GroupKeys", len, distinct), reg);
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
BENCHMARK(BM_Execute_GroupKeys)->Args({4096, 16})->Args({4096, 256})->Args({4096, 4096});

// C-group-4: GroupSum sugar — scatter-add with int-to-i64 promotion.
void BM_Execute_GroupSum(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  const int distinct = static_cast<int>(state.range(1));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeGroupSugarCall("GroupSum", len, distinct), reg);
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
BENCHMARK(BM_Execute_GroupSum)->Args({4096, 16})->Args({4096, 256})->Args({4096, 4096});

// C-group-5: GroupMax sugar — scatter-max with lowest() seed.
void BM_Execute_GroupMax(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  const int distinct = static_cast<int>(state.range(1));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeGroupSugarCall("GroupMax", len, distinct), reg);
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
BENCHMARK(BM_Execute_GroupMax)->Args({4096, 16})->Args({4096, 256})->Args({4096, 4096});

// C-group-6: GroupMin sugar — scatter-min with max() seed.
void BM_Execute_GroupMin(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  const int distinct = static_cast<int>(state.range(1));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeGroupSugarCall("GroupMin", len, distinct), reg);
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
BENCHMARK(BM_Execute_GroupMin)->Args({4096, 16})->Args({4096, 256})->Args({4096, 4096});

// C-group-7: GroupAvg sugar — scatter-sum + per-bucket divide (f64 output).
void BM_Execute_GroupAvg(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  const int distinct = static_cast<int>(state.range(1));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeGroupSugarCall("GroupAvg", len, distinct), reg);
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
BENCHMARK(BM_Execute_GroupAvg)->Args({4096, 16})->Args({4096, 256})->Args({4096, 4096});

}  // namespace

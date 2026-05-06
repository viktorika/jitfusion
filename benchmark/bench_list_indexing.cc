// K. List indexing kernels: lookup / compact / gather.
//
// Kernel overview:
//   ListLookupIndex(a, b, ctx)         : for each element in a, find its first
//                                        occurrence in b, return the position
//                                        (u32max on miss). Built on a hash
//                                        table over b.
//   ListCompactPositions(raw, ctx)     : given a U32 list produced by
//                                        ListLookupIndex, return the positions
//                                        in `a` that hit (i.e. the indices
//                                        themselves, filtered by "not miss").
//   ListCompactIndex(raw, ctx)         : same filter, but returns the looked-up
//                                        b-side indices directly.
//   ListGather(values, idx, dflt, ctx) : out[k] = (idx[k] < values.len) ?
//                                        values[idx[k]] : dflt. The typical
//                                        join-realign kernel.

#include <cstdint>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "benchmark/bench_common.h"

namespace {

using ::jitfusion::ConstantListValueNode;
using ::jitfusion::ConstantValueNode;
using ::jitfusion::ExecContext;
using ::jitfusion::ExecNode;
using ::jitfusion::FunctionNode;
using ::jitfusion::RetType;
using ::jitfusion::bench::CompileOrDie;
using ::jitfusion::bench::MakeRegistry;

// Build `ListLookupIndex(a, b, ctx)` where both are i64 lists of length `len`.
// Half of a's keys hit in b (pattern chosen to exercise both hit and miss
// branches of the inner loop).
std::unique_ptr<ExecNode> MakeLookupIndexCall(int len) {
  std::vector<int64_t> a;
  std::vector<int64_t> b;
  a.reserve(len);
  b.reserve(len);
  for (int i = 0; i < len; ++i) {
    a.push_back(i);                  // keys 0..len-1
    b.push_back(i * 2);              // keys 0,2,4,... — only even keys from a will hit
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(a)));
  args.emplace_back(new ConstantListValueNode(std::move(b)));
  args.emplace_back(new jitfusion::ExecContextNode());
  return std::unique_ptr<ExecNode>(new FunctionNode("ListLookupIndex", std::move(args)));
}

void BM_Execute_ListLookupIndex(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  auto engine = CompileOrDie(MakeLookupIndexCall(len), reg);
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
BENCHMARK(BM_Execute_ListLookupIndex)->Arg(256)->Arg(4096);

// Build a raw U32 list that alternates between a hit (some valid index) and
// a miss (u32max) — matches the expected shape of a ListLookupIndex output and
// lets the compact kernel do real work.
std::unique_ptr<ExecNode> MakeRawLookupList(int len) {
  std::vector<uint32_t> raw;
  raw.reserve(len);
  constexpr uint32_t kMiss = std::numeric_limits<uint32_t>::max();
  for (int i = 0; i < len; ++i) {
    raw.push_back((i & 1) == 0 ? static_cast<uint32_t>(i / 2) : kMiss);
  }
  return std::unique_ptr<ExecNode>(new ConstantListValueNode(std::move(raw)));
}

void BM_Execute_ListCompactPositions(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(MakeRawLookupList(len));
  args.emplace_back(new jitfusion::ExecContextNode());
  std::unique_ptr<ExecNode> node(new FunctionNode("ListCompactPositions", std::move(args)));

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
BENCHMARK(BM_Execute_ListCompactPositions)->Arg(256)->Arg(4096);

void BM_Execute_ListCompactIndex(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(MakeRawLookupList(len));
  args.emplace_back(new jitfusion::ExecContextNode());
  std::unique_ptr<ExecNode> node(new FunctionNode("ListCompactIndex", std::move(args)));

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
BENCHMARK(BM_Execute_ListCompactIndex)->Arg(256)->Arg(4096);

// ListGather(values<i64>, idx<u32>, default<i64>, ctx). `idx` is arranged so
// every lookup hits (j < values.len) — the common hot path.
void BM_Execute_ListGather(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  std::vector<int64_t> values;
  std::vector<uint32_t> idx;
  values.reserve(len);
  idx.reserve(len);
  for (int i = 0; i < len; ++i) {
    values.push_back(i * 10);
    idx.push_back(static_cast<uint32_t>(len - 1 - i));  // reverse permutation
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(values)));
  args.emplace_back(new ConstantListValueNode(std::move(idx)));
  args.emplace_back(new ConstantValueNode(static_cast<int64_t>(-1)));
  args.emplace_back(new jitfusion::ExecContextNode());
  std::unique_ptr<ExecNode> node(new FunctionNode("ListGather", std::move(args)));

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
BENCHMARK(BM_Execute_ListGather)->Arg(256)->Arg(4096);

}  // namespace

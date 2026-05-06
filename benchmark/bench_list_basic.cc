// G. List basic (shape manipulation) + H. List element-wise unary.
// See bench_compile.cc for the split overview.

#include "benchmark/bench_common.h"

namespace {

using ::jitfusion::bench::CompileOrDie;
using ::jitfusion::bench::MakeListListCtxCall;
using ::jitfusion::bench::MakeListUnaryCall;
using ::jitfusion::bench::MakeListUnaryCtxCall;
using ::jitfusion::bench::MakeListUnaryCtxCallF64;
using ::jitfusion::bench::MakeRegistry;
using ::jitfusion::ConstantListValueNode;
using ::jitfusion::ConstantValueNode;
using ::jitfusion::ExecContext;
using ::jitfusion::ExecNode;
using ::jitfusion::FunctionNode;
using ::jitfusion::RetType;

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

// CrossJoin(stringlist a, stringlist b, string sep, ctx) — produces a
// length-(|a| * |b|) stringlist of "a[i] + sep + b[j]". Output size is
// quadratic in the input length, so we sweep the per-side length from 16..256
// (yielding 256..65536 output elements) instead of the usual 256/4096 sweep
// used for linear-cost kernels.
void BM_Execute_CrossJoin(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  std::vector<std::string> a;
  std::vector<std::string> b;
  a.reserve(len);
  b.reserve(len);
  for (int i = 0; i < len; ++i) {
    a.push_back("a" + std::to_string(i));
    b.push_back("b" + std::to_string(i));
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(a)));
  args.emplace_back(new ConstantListValueNode(std::move(b)));
  args.emplace_back(new ConstantValueNode(std::string("_")));
  args.emplace_back(new jitfusion::ExecContextNode());
  std::unique_ptr<ExecNode> node(new FunctionNode("CrossJoin", std::move(args)));

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
BENCHMARK(BM_Execute_CrossJoin)->Arg(16)->Arg(64)->Arg(256);

// ZipConcat(stringlist a, stringlist b, string sep, ctx) — per-position
// concatenation, output length equals |a| (== |b|). Linear cost in input
// length, so it shares the standard 256/4096 sweep with the other linear
// list kernels.
void BM_Execute_ZipConcat(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  auto reg = MakeRegistry();
  std::vector<std::string> a;
  std::vector<std::string> b;
  a.reserve(len);
  b.reserve(len);
  for (int i = 0; i < len; ++i) {
    a.push_back("a" + std::to_string(i));
    b.push_back("b" + std::to_string(i));
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(a)));
  args.emplace_back(new ConstantListValueNode(std::move(b)));
  args.emplace_back(new ConstantValueNode(std::string(":")));
  args.emplace_back(new jitfusion::ExecContextNode());
  std::unique_ptr<ExecNode> node(new FunctionNode("ZipConcat", std::move(args)));

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
BENCHMARK(BM_Execute_ZipConcat)->Arg(256)->Arg(4096);

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

}  // namespace

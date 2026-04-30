// I. List element-wise binary — two shapes (scalar broadcast + list-list zip).
// See bench_compile.cc for the split overview.

#include "benchmark/bench_common.h"

namespace {

using ::jitfusion::bench::CompileOrDie;
using ::jitfusion::bench::MakeListListCtxCall;
using ::jitfusion::bench::MakeListListCtxCallU64;
using ::jitfusion::bench::MakeListScalarCtxCall;
using ::jitfusion::bench::MakeListScalarCtxCallU64;
using ::jitfusion::bench::MakeRegistry;
using ::jitfusion::ExecContext;
using ::jitfusion::RetType;

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
#undef JITFUSION_DEFINE_LIST_BINARY_BM

// ListMax / ListMin are only registered in the list-scalar (broadcast) form:
// there is no list-list overload in the current function registry. Exercising
// the `_LL` variant would cause Compile() to fail (no matching signature),
// which in turn aborts the whole benchmark binary via CompileOrDie(). Keep
// only the scalar shape here — if list-list versions get added later, move
// these two back into JITFUSION_DEFINE_LIST_BINARY_BM.
#define JITFUSION_DEFINE_LIST_BINARY_SCALAR_ONLY_BM(NAME)                  \
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
  BENCHMARK(BM_Execute_##NAME##_Scalar)->Arg(4096)  // NOLINT

JITFUSION_DEFINE_LIST_BINARY_SCALAR_ONLY_BM(ListMax);
JITFUSION_DEFINE_LIST_BINARY_SCALAR_ONLY_BM(ListMin);
#undef JITFUSION_DEFINE_LIST_BINARY_SCALAR_ONLY_BM

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

}  // namespace

// J. List comparison / bitmap kernels. See bench_compile.cc for the overview.

#include "benchmark/bench_common.h"

namespace {

using ::jitfusion::ConstantListValueNode;
using ::jitfusion::ConstantValueNode;
using ::jitfusion::ExecContext;
using ::jitfusion::ExecNode;
using ::jitfusion::FunctionNode;
using ::jitfusion::RetType;
using ::jitfusion::bench::CompileOrDie;
using ::jitfusion::bench::MakeListIfSelectCall;
using ::jitfusion::bench::MakeListScalarCtxCall;
using ::jitfusion::bench::MakeRegistry;

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

// IfByBitmap(packed_bitmap<u8>, value_list<i64>, alt_scalar<i64>, ctx).
// For each element i: packed_bitmap bit i set ? value_list[i] : alt_scalar.
// `packed_bitmap` is a U8List of ceil(len/8) bytes, LSB = first element.
void BM_Execute_IfByBitmap(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  const int bitmap_bytes = (len + 7) / 8;
  auto reg = MakeRegistry();
  std::vector<uint8_t> bitmap(bitmap_bytes, static_cast<uint8_t>(0x55));  // every other bit set
  std::vector<int64_t> values(len);
  for (int i = 0; i < len; ++i) {
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

// FilterByBitmap(value_list<i64>, packed_bitmap<u8>, ctx) — sugar form.
//
// The `packed_bitmap` is ceil(values.size() / 8) bytes; each byte encodes 8
// elements (LSB = first element). 0x55 (0b01010101) gives 4 ones per byte →
// every other element is kept. The 3-arg sugar auto-derives `bits_cnt` via
// CountBits(bitmap) at codegen time, so we no longer have to precompute and
// pass the popcount explicitly (and can no longer get it wrong).
void BM_Execute_FilterByBitmap(benchmark::State& state) {
  const int len = static_cast<int>(state.range(0));
  const int bitmap_bytes = (len + 7) / 8;
  auto reg = MakeRegistry();
  std::vector<int64_t> values(len);
  std::vector<uint8_t> bitmap(bitmap_bytes, static_cast<uint8_t>(0x55));
  for (int i = 0; i < len; ++i) {
    values[i] = i;
  }
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new ConstantListValueNode(std::move(values)));
  args.emplace_back(new ConstantListValueNode(std::move(bitmap)));
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

}  // namespace

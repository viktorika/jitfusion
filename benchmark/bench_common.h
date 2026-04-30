// Shared AST/registry helpers for JitFusion benchmark TUs.
//
// Splitting engine_benchmark.cc into per-category files (bench_compile.cc,
// bench_execute_core.cc, bench_list_agg.cc, ...) is purely an organisational
// change. Every TU needs the same AST-construction and registry helpers, so
// we centralise them here.
//
// Adding new benchmarks: drop a new bench_*.cc file into this directory.
// Both BUILD (glob) and CMakeLists.txt (file(GLOB ...)) pick it up
// automatically — you don't need to edit any build script.
//
// IMPORTANT: `BENCHMARK_MAIN()` must appear in exactly one TU. It currently
// lives at the bottom of bench_compile.cc. Do NOT add another BENCHMARK_MAIN
// in any other bench_*.cc — the glob-based srcs will link them all together
// and you'll get `multiple definition of main`.
//
// Every helper is marked `inline` so each TU can include this header without
// multiple-definition errors; the linker folds the copies together.
// `StoreI32` is also `inline` because some benchmarks register its address
// into a FunctionRegistry; `inline` (C++17) guarantees a single canonical
// address across the whole binary, which matters for the JIT — otherwise
// different TUs could pick different function addresses.

#ifndef JITFUSION_BENCHMARK_BENCH_COMMON_H_
#define JITFUSION_BENCHMARK_BENCH_COMMON_H_

#include <benchmark/benchmark.h>

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "exec_engine.h"
#include "exec_node.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace jitfusion {
namespace bench {

using ::jitfusion::BinaryOPNode;
using ::jitfusion::BinaryOPType;
using ::jitfusion::ConstantListValueNode;
using ::jitfusion::ConstantValueNode;
using ::jitfusion::ExecContext;
using ::jitfusion::ExecEngine;
using ::jitfusion::ExecEngineOption;
using ::jitfusion::ExecNode;
using ::jitfusion::FunctionNode;
using ::jitfusion::FunctionRegistry;
using ::jitfusion::FunctionRegistryFactory;
using ::jitfusion::FunctionSignature;
using ::jitfusion::IfBlockNode;
using ::jitfusion::IfNode;
using ::jitfusion::NoOPNode;
using ::jitfusion::OutputNode;
using ::jitfusion::RefNode;
using ::jitfusion::RetType;
using ::jitfusion::SwitchNode;
using ::jitfusion::ValueType;

// -----------------------------------------------------------------------------
// AST construction helpers
// -----------------------------------------------------------------------------

// Linear chain: ((((c0 + c1) + c2) + c3) + ...)
inline std::unique_ptr<ExecNode> MakeLinearAddChain(int depth) {
  std::unique_ptr<ExecNode> cur(new ConstantValueNode(static_cast<int64_t>(0)));
  for (int i = 1; i <= depth; ++i) {
    std::unique_ptr<ExecNode> rhs(new ConstantValueNode(static_cast<int64_t>(i)));
    cur = std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kAdd, std::move(cur), std::move(rhs)));
  }
  return cur;
}

// Deeply nested if-else chain: if(c,1,if(c,2,if(c,3,...)))
inline std::unique_ptr<ExecNode> MakeDeepNestedIf(int depth) {
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
inline std::unique_ptr<ExecNode> MakeManyAbsCalls(int depth) {
  std::unique_ptr<ExecNode> cur(new ConstantValueNode(static_cast<int32_t>(-42)));
  for (int i = 0; i < depth; ++i) {
    std::vector<std::unique_ptr<ExecNode>> args;
    args.emplace_back(std::move(cur));
    cur = std::unique_ptr<ExecNode>(new FunctionNode("abs", std::move(args)));
  }
  return cur;
}

// Build `func_name(const_list_of_length(list_len))`.
inline std::unique_ptr<ExecNode> MakeListUnaryCall(const std::string& func_name, int list_len) {
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
inline std::unique_ptr<ExecNode> MakeListSumExpr(int list_len) { return MakeListUnaryCall("Sum", list_len); }

// Build `func_name(list, exec_ctx)` — one-arg list kernels that require an
// ExecContext pointer (arithmetic/basic kernels that allocate output lists).
inline std::unique_ptr<ExecNode> MakeListUnaryCtxCall(const std::string& func_name, int list_len) {
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
inline std::unique_ptr<ExecNode> MakeListUnaryCtxCallF64(const std::string& func_name, int list_len) {
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
inline std::unique_ptr<ExecNode> MakeListScalarCtxCall(const std::string& func_name, int list_len, int64_t scalar_val) {
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
inline std::unique_ptr<ExecNode> MakeListScalarCtxCallU64(const std::string& func_name, int list_len,
                                                         uint64_t scalar_val) {
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
inline std::unique_ptr<ExecNode> MakeListListCtxCallU64(const std::string& func_name, int list_len) {
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
inline std::unique_ptr<ExecNode> MakeListListCtxCall(const std::string& func_name, int list_len) {
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
inline std::unique_ptr<ExecNode> MakeListIfSelectCall(const std::string& func_name, int list_len) {
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

inline std::unique_ptr<FunctionRegistry> MakeRegistry() {
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
//
// Must be `inline` so its address is unique across TUs that include this
// header — the JIT registry stores this address and the generated IR calls
// it by pointer.
inline void StoreI32(void* output, int32_t index, int32_t value) {
  reinterpret_cast<int32_t*>(output)[index] = value;
}

inline std::unique_ptr<FunctionRegistry> MakeRegistryWithStore() {
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
inline std::unique_ptr<ExecNode> MakeStoreI32(int32_t index, std::unique_ptr<ExecNode> value) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(new OutputNode());
  args.emplace_back(new ConstantValueNode(index));
  args.emplace_back(std::move(value));
  return std::unique_ptr<ExecNode>(new FunctionNode("store", std::move(args)));
}

// Compile one AST or abort. Keeps benchmark code free of ASSERT noise.
inline std::unique_ptr<ExecEngine> CompileOrDie(std::unique_ptr<ExecNode> node,
                                                const std::unique_ptr<FunctionRegistry>& reg, bool dump_ir = false) {
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

}  // namespace bench
}  // namespace jitfusion

#endif  // JITFUSION_BENCHMARK_BENCH_COMMON_H_

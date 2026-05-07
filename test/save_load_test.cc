/*
 * @Author: victorika 
 * @Date: 2026-05-07 16:13:43 
 * @Last Modified by:   victorika 
 * @Last Modified time: 2026-05-07 16:13:43 
 */
/*
 * End-to-end tests for ExecEngine::SaveCompiled / LoadCompiled.
 *
 * Covers the intended round-trip contract:
 *   1. Simple scalar expression.
 *   2. Expression that uses a ReadOnlyCFunc — Load must reuse the freshly
 *      supplied registry addresses, not any captured-at-save addresses.
 *   3. Expression with constant list / string payloads — exercises the
 *      relocatable-constant globals path end-to-end (SaveCompiled captures
 *      the relocatable .o, LoadCompiled links it in a fresh JIT).
 *   4. BatchCompile round-trip with multiple entry symbols / ret types.
 *   5. Negative paths: bad magic, truncated blob, LLVM version mismatch,
 *      missing function on Load side, SaveCompiled after LoadCompiled.
 */
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "exec_engine.h"
#include "exec_node.h"
#include "function_registry.h"
#include "gtest/gtest.h"
#include "status.h"
#include "type.h"

using namespace jitfusion;

namespace {

int32_t LoadI32(void* data, int32_t i) { return reinterpret_cast<int32_t*>(data)[i]; }

std::unique_ptr<FunctionRegistry> MakeRegistry() {
  std::unique_ptr<FunctionRegistry> reg;
  FunctionRegistryFactory::CreateFunctionRegistry(&reg);
  return reg;
}

std::unique_ptr<FunctionRegistry> MakeRegistryWithLoadI32() {
  auto reg = MakeRegistry();
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32);
  EXPECT_TRUE(reg->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32)).ok());
  return reg;
}

// Build an ExecEngine configured so that SaveCompiled is usable after
// Compile. Using this everywhere makes the opt-in nature of save
// capture explicit in every test, and catches regressions where a new
// code path forgets to set the option.
ExecEngineOption SaveCapableOption() {
  ExecEngineOption opt;
  opt.enable_save_compiled = true;
  return opt;
}

}  // namespace

// ---------------------------------------------------------------------------
// Scenario 1: trivial scalar constant — no functions, no payloads.
// Purpose: confirm the wire format is well-formed for the smallest case
// and that LoadCompiled produces an engine that behaves identically.
// ---------------------------------------------------------------------------
TEST(SaveLoadTest, ScalarConstantRoundTrip) {
  auto reg = MakeRegistry();
  auto node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(42)));

  ExecEngine src(SaveCapableOption());
  ASSERT_TRUE(src.Compile(node, reg).ok());

  std::string blob;
  ASSERT_TRUE(src.SaveCompiled(&blob).ok()) << "SaveCompiled failed";
  EXPECT_GT(blob.size(), 32u) << "Artifact should be at least header + object bytes";

  // Load into a completely separate engine + registry to prove no hidden
  // state leaks from the source engine.
  auto reg2 = MakeRegistry();
  ExecEngine dst;
  ASSERT_TRUE(dst.LoadCompiled(blob, reg2).ok()) << "LoadCompiled failed";

  RetType r;
  ASSERT_TRUE(dst.Execute(nullptr, &r).ok());
  EXPECT_EQ(std::get<int32_t>(r), 42);
}

// ---------------------------------------------------------------------------
// Scenario 2: expression that calls a ReadOnlyCFunc.
// Purpose: the saved .o contains an undefined reference to "load(...)";
// LoadCompiled must resolve it from the Load-side registry, not from any
// address encoded at Save time. We demonstrate this by giving the Load
// side a freshly-constructed registry (different heap address than the
// Save side, though pointing to the same C function).
// ---------------------------------------------------------------------------
TEST(SaveLoadTest, ReadOnlyCFuncResolvesOnLoadSideRegistry) {
  auto reg_save = MakeRegistryWithLoadI32();

  auto arg = std::unique_ptr<ExecNode>(new EntryArgumentNode);
  auto idx = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(2)));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(arg));
  args.emplace_back(std::move(idx));
  auto call = std::unique_ptr<ExecNode>(new FunctionNode("load", std::move(args)));

  ExecEngine src(SaveCapableOption());
  ASSERT_TRUE(src.Compile(call, reg_save).ok());

  std::string blob;
  ASSERT_TRUE(src.SaveCompiled(&blob).ok());

  auto reg_load = MakeRegistryWithLoadI32();  // distinct registry instance
  ExecEngine dst;
  ASSERT_TRUE(dst.LoadCompiled(blob, reg_load).ok());

  std::vector<int32_t> input = {10, 20, 30, 40};
  RetType r;
  ASSERT_TRUE(dst.Execute(input.data(), &r).ok());
  EXPECT_EQ(std::get<int32_t>(r), 30);
}

// ---------------------------------------------------------------------------
// Scenario 3: constant list + string payloads.
// Purpose: this is the exact path that required the "relocatable globals"
// refactor. If the codegen ever regresses to inttoptr-based payloads, the
// .o file would embed process-local addresses and LoadCompiled would
// either crash or produce garbage. Running SaveCompiled + LoadCompiled in
// the same process still exercises the serialization/deserialization
// logic, and — since the globals are baked into the .o — proves they
// travel with the artifact rather than being resolved from live memory.
// ---------------------------------------------------------------------------
TEST(SaveLoadTest, ConstantStringAndListRoundTrip) {
  // String literal.
  {
    auto reg = MakeRegistry();
    auto node = std::unique_ptr<ExecNode>(new ConstantValueNode(std::string("jitfusion")));
    ExecEngine src(SaveCapableOption());
    ASSERT_TRUE(src.Compile(node, reg).ok());
    std::string blob;
    ASSERT_TRUE(src.SaveCompiled(&blob).ok());

    ExecEngine dst;
    auto reg2 = MakeRegistry();
    ASSERT_TRUE(dst.LoadCompiled(blob, reg2).ok());
    RetType r;
    ASSERT_TRUE(dst.Execute(nullptr, &r).ok());
    EXPECT_EQ(std::get<std::string>(r), "jitfusion");
  }
  // Numeric list (exercises EmitTypedArrayGlobal).
  {
    auto reg = MakeRegistry();
    std::vector<int32_t> data = {7, 8, 9, 10, 11, 12};
    auto node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
    ExecEngine src(SaveCapableOption());
    ASSERT_TRUE(src.Compile(node, reg).ok());
    std::string blob;
    ASSERT_TRUE(src.SaveCompiled(&blob).ok());

    ExecEngine dst;
    auto reg2 = MakeRegistry();
    ASSERT_TRUE(dst.LoadCompiled(blob, reg2).ok());
    RetType r;
    ASSERT_TRUE(dst.Execute(nullptr, &r).ok());
    EXPECT_EQ(std::get<std::vector<int32_t>>(r), data);
  }
  // String list (exercises the two-level global path).
  {
    auto reg = MakeRegistry();
    std::vector<std::string> data = {"alpha", "beta", "", "gamma"};
    auto node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
    ExecEngine src(SaveCapableOption());
    ASSERT_TRUE(src.Compile(node, reg).ok());
    std::string blob;
    ASSERT_TRUE(src.SaveCompiled(&blob).ok());

    ExecEngine dst;
    auto reg2 = MakeRegistry();
    ASSERT_TRUE(dst.LoadCompiled(blob, reg2).ok());
    RetType r;
    ASSERT_TRUE(dst.Execute(nullptr, &r).ok());
    EXPECT_EQ(std::get<std::vector<std::string>>(r), data);
  }
}

// ---------------------------------------------------------------------------
// Scenario 4: BatchCompile round-trip.
// Purpose: exercises the multi-symbol + per-entry ret_type path through
// the format. We deliberately mix return types so a missing per-entry
// ret_type would surface as a type-mismatched Execute.
// ---------------------------------------------------------------------------
TEST(SaveLoadTest, BatchCompileRoundTrip) {
  auto reg = MakeRegistry();
  std::vector<std::unique_ptr<ExecNode>> nodes;
  nodes.emplace_back(new ConstantValueNode(static_cast<int32_t>(111)));
  nodes.emplace_back(new ConstantValueNode(static_cast<double>(2.5)));
  nodes.emplace_back(new ConstantValueNode(std::string("batched")));

  ExecEngine src(SaveCapableOption());
  ASSERT_TRUE(src.BatchCompile(nodes, reg).ok());
  ASSERT_EQ(src.GetBatchFunctionCount(), 3u);

  std::string blob;
  ASSERT_TRUE(src.SaveCompiled(&blob).ok());

  ExecEngine dst;
  auto reg2 = MakeRegistry();
  ASSERT_TRUE(dst.LoadCompiled(blob, reg2).ok());
  ASSERT_EQ(dst.GetBatchFunctionCount(), 3u);
  EXPECT_EQ(dst.GetBatchFunctionReturnType(0), ValueType::kI32);
  EXPECT_EQ(dst.GetBatchFunctionReturnType(1), ValueType::kF64);
  EXPECT_EQ(dst.GetBatchFunctionReturnType(2), ValueType::kString);

  RetType r;
  ASSERT_TRUE(dst.ExecuteAt(0, nullptr, &r).ok());
  EXPECT_EQ(std::get<int32_t>(r), 111);
  ASSERT_TRUE(dst.ExecuteAt(1, nullptr, &r).ok());
  EXPECT_DOUBLE_EQ(std::get<double>(r), 2.5);
  ASSERT_TRUE(dst.ExecuteAt(2, nullptr, &r).ok());
  EXPECT_EQ(std::get<std::string>(r), "batched");
}

// ---------------------------------------------------------------------------
// Scenario 5a: SaveCompiled before Compile is an error (nothing to save).
// ---------------------------------------------------------------------------
TEST(SaveLoadTest, SaveBeforeCompileFails) {
  ExecEngine engine;
  std::string blob;
  auto st = engine.SaveCompiled(&blob);
  EXPECT_FALSE(st.ok()) << st.ToString();
}

// ---------------------------------------------------------------------------
// Scenario 5a': Compile ran, but enable_save_compiled was left at its
// default (false) — SaveCompiled must fail with a message that points at
// the opt-in, not just "nothing to save". We verify the message mentions
// the option name so users can fix their setup without reading source.
// ---------------------------------------------------------------------------
TEST(SaveLoadTest, SaveWithoutOptInFails) {
  auto reg = MakeRegistry();
  auto node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(1)));
  ExecEngine engine;  // default option: enable_save_compiled=false
  ASSERT_TRUE(engine.Compile(node, reg).ok());

  std::string blob;
  auto st = engine.SaveCompiled(&blob);
  ASSERT_FALSE(st.ok());
  // Spot-check that the error message is actionable. We don't pin the
  // exact wording, just the identifier of the option the user needs to
  // flip — that's what makes the message useful.
  EXPECT_NE(st.ToString().find("enable_save_compiled"), std::string::npos)
      << "Error should reference enable_save_compiled. Actual: " << st.ToString();
}

// ---------------------------------------------------------------------------
// Scenario 5b: Re-save after Load is rejected (we deliberately do not
// keep the loaded bytes around — documented in the header).
// ---------------------------------------------------------------------------
TEST(SaveLoadTest, ResaveAfterLoadFails) {
  auto reg = MakeRegistry();
  auto node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(1)));
  ExecEngine src(SaveCapableOption());
  ASSERT_TRUE(src.Compile(node, reg).ok());
  std::string blob;
  ASSERT_TRUE(src.SaveCompiled(&blob).ok());

  // Note: dst intentionally does NOT enable_save_compiled — LoadCompiled
  // shouldn't care about that option, and we want to verify the re-save
  // block still kicks in. Flipping the option on the load side would not
  // change the outcome because LoadCompiled deliberately leaves
  // compiled_object_bytes_ empty.
  ExecEngine dst;
  ASSERT_TRUE(dst.LoadCompiled(blob, reg).ok());

  std::string blob2;
  auto st = dst.SaveCompiled(&blob2);
  EXPECT_FALSE(st.ok()) << "Re-save after load should be rejected; got: " << st.ToString();
}

// ---------------------------------------------------------------------------
// Scenario 5c: corrupted / foreign input is rejected cleanly
// (no crash, meaningful error message).
// ---------------------------------------------------------------------------
TEST(SaveLoadTest, LoadRejectsGarbage) {
  auto reg = MakeRegistry();
  ExecEngine dst;

  // Completely empty
  EXPECT_FALSE(dst.LoadCompiled("", reg).ok());
  // Too short to contain even the magic
  EXPECT_FALSE(dst.LoadCompiled("JF", reg).ok());
  // Wrong magic
  EXPECT_FALSE(dst.LoadCompiled("ZZZZZZZZZZZZZZZZ", reg).ok());
}

TEST(SaveLoadTest, LoadRejectsTruncatedBlob) {
  auto reg = MakeRegistry();
  auto node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(1)));
  ExecEngine src(SaveCapableOption());
  ASSERT_TRUE(src.Compile(node, reg).ok());
  std::string blob;
  ASSERT_TRUE(src.SaveCompiled(&blob).ok());

  // Drop the footer and some of the object bytes. The parser should
  // surface a truncation error on the footer read, not crash.
  std::string truncated = blob.substr(0, blob.size() - 8);
  ExecEngine dst;
  auto st = dst.LoadCompiled(truncated, reg);
  EXPECT_FALSE(st.ok()) << "Truncated blob should be rejected";
}

TEST(SaveLoadTest, LoadRejectsTamperedLLVMVersion) {
  auto reg = MakeRegistry();
  auto node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(1)));
  ExecEngine src(SaveCapableOption());
  ASSERT_TRUE(src.Compile(node, reg).ok());
  std::string blob;
  ASSERT_TRUE(src.SaveCompiled(&blob).ok());

  // The llvm version string lives right after magic(4) + format_version(4).
  // Its length prefix is at offset 8. Mutate one byte of the payload so
  // the stored version no longer matches the runtime's.
  ASSERT_GT(blob.size(), 20u);
  // offset 8..11: u32 length. offset 12: first byte of llvm version string.
  // Flip first character of the version string; will cause strict compare
  // to fail.
  blob[12] = (blob[12] == 'X' ? 'Y' : 'X');
  ExecEngine dst;
  auto st = dst.LoadCompiled(blob, reg);
  EXPECT_FALSE(st.ok()) << "Tampered LLVM version must be rejected; got: " << st.ToString();
}

// ---------------------------------------------------------------------------
// Scenario 5d: Load-side registry missing the function referenced by the
// artifact. Must fail at LoadCompiled (symbol lookup stage), not silently
// succeed and then crash on Execute.
// ---------------------------------------------------------------------------
TEST(SaveLoadTest, LoadFailsWhenRegistryMissesFunction) {
  auto reg_save = MakeRegistryWithLoadI32();
  auto arg = std::unique_ptr<ExecNode>(new EntryArgumentNode);
  auto idx = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(0)));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(arg));
  args.emplace_back(std::move(idx));
  auto call = std::unique_ptr<ExecNode>(new FunctionNode("load", std::move(args)));

  ExecEngine src(SaveCapableOption());
  ASSERT_TRUE(src.Compile(call, reg_save).ok());
  std::string blob;
  ASSERT_TRUE(src.SaveCompiled(&blob).ok());

  // Load side supplies a bare registry with no "load" function.
  auto reg_load = MakeRegistry();
  ExecEngine dst;
  auto st = dst.LoadCompiled(blob, reg_load);
  EXPECT_FALSE(st.ok()) << "Load should fail fast if a required function is missing; got: " << st.ToString();
}

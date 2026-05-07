/*
 * @Author: victorika
 * @Date: 2026-05-07 14:30:30
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 14:30:30
 */
/*
 * Regression tests for the "relocatable constants" refactor.
 *
 * Goal: guarantee that the LLVM Module produced by Compile() /
 * BatchCompile() never embeds absolute host addresses for constant
 * string / list payloads. Such addresses would manifest as
 * `inttoptr (i64 <nonzero-host-addr> to ptr)` in the IR dump and would
 * make the module unusable across processes — which is the foundation
 * a future ObjectCache layer depends on.
 *
 * We check two invariants on the dumped, fully-optimized IR:
 *   1. It contains at least one private unnamed_addr constant global
 *      carrying the constant payload (proving the new emission path
 *      actually runs).
 *   2. It contains no `inttoptr` form with a nonzero host address
 *      pointing into the current process. We allow `inttoptr i64 0`
 *      because the null pointer is a legitimate constant encoding
 *      for empty string/list.
 */
#include <cstdint>
#include <memory>
#include <regex>
#include <string>
#include <vector>
#include "exec_engine.h"
#include "exec_node.h"
#include "function_registry.h"
#include "gtest/gtest.h"
#include "type.h"

using namespace jitfusion;

namespace {

// Returns true if `ir` contains any `inttoptr` whose integer operand is
// a nonzero literal. This matches both the legacy form
// `inttoptr (i64 12345 to ptr)` and the `inttoptr i64 12345 to ptr`
// instruction form. `inttoptr i64 0 to ptr` is explicitly treated as
// acceptable (that is how LLVM spells an empty StringStruct/ListStruct
// data pointer in our encoding, and it is trivially relocatable).
bool HasNonzeroInttoptr(const std::string& ir) {
  // The regex is intentionally permissive about spacing and the
  // optional surrounding parentheses because LLVM's printer is
  // inconsistent between ConstantExpr and Instruction forms.
  std::regex re(R"(inttoptr\s*\(?\s*i(?:8|16|32|64)\s+(-?\d+))");
  auto begin = std::sregex_iterator(ir.begin(), ir.end(), re);
  auto end = std::sregex_iterator();
  for (auto it = begin; it != end; ++it) {
    const std::string& num = (*it)[1];
    if (num != "0") {
      return true;
    }
  }
  return false;
}

bool HasPrivateConstantGlobal(const std::string& ir) {
  // The globals emitted by EmitStringPayloadGlobal / EmitRawBytesGlobal
  // share the "jf." name prefix and always have private+constant
  // linkage. Post-optimization LLVM may rename (e.g. jf.str.1) and may
  // inline some into `.rodata` merged constants, but at least one of
  // the shapes below should survive for any non-trivial constant.
  return ir.find("private unnamed_addr constant") != std::string::npos ||
         ir.find("internal unnamed_addr constant") != std::string::npos;
}

ExecEngineOption DumpIROpt() {
  ExecEngineOption opt;
  opt.dump_ir = true;
  return opt;
}

}  // namespace

TEST(RelocatableConstTest, StringLiteralUsesGlobal) {
  std::unique_ptr<FunctionRegistry> reg;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&reg).ok());

  const std::string literal = "hello_relocatable_world";
  auto node = std::unique_ptr<ExecNode>(new ConstantValueNode(literal));

  ExecEngine engine(DumpIROpt());
  ASSERT_TRUE(engine.Compile(node, reg).ok());

  std::string ir(engine.GetIRCode());
  ASSERT_FALSE(ir.empty());
  EXPECT_TRUE(HasPrivateConstantGlobal(ir)) << "IR should contain a private constant global for the string payload:\n"
                                            << ir;
  EXPECT_FALSE(HasNonzeroInttoptr(ir))
      << "IR still embeds an absolute host address via `inttoptr`; the module is not relocatable:\n"
      << ir;

  // And the runtime semantics must not regress.
  RetType result;
  ASSERT_TRUE(engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::string>(result), literal);
}

TEST(RelocatableConstTest, NumericListUsesGlobal) {
  std::unique_ptr<FunctionRegistry> reg;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&reg).ok());

  std::vector<int32_t> data = {1, 2, 3, 4, 5, 6, 7, 8};
  auto node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));

  ExecEngine engine(DumpIROpt());
  ASSERT_TRUE(engine.Compile(node, reg).ok());

  std::string ir(engine.GetIRCode());
  ASSERT_FALSE(ir.empty());
  EXPECT_TRUE(HasPrivateConstantGlobal(ir)) << ir;
  EXPECT_FALSE(HasNonzeroInttoptr(ir)) << ir;

  RetType result;
  ASSERT_TRUE(engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), data);
}

TEST(RelocatableConstTest, StringListUsesGlobal) {
  std::unique_ptr<FunctionRegistry> reg;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&reg).ok());

  std::vector<std::string> data = {"alpha", "beta", "", "gamma_with_more_bytes"};
  auto node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));

  ExecEngine engine(DumpIROpt());
  ASSERT_TRUE(engine.Compile(node, reg).ok());

  std::string ir(engine.GetIRCode());
  ASSERT_FALSE(ir.empty());
  EXPECT_TRUE(HasPrivateConstantGlobal(ir)) << ir;
  EXPECT_FALSE(HasNonzeroInttoptr(ir)) << ir;

  RetType result;
  ASSERT_TRUE(engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::vector<std::string>>(result), data);
}

TEST(RelocatableConstTest, EmptyStringAndEmptyListStillWork) {
  // Empty payloads intentionally serialize to {data=null, len=0},
  // which shows up as `inttoptr i64 0` or just `ptr null` in IR.
  // Both forms are relocatable; this test guards the corner case.
  std::unique_ptr<FunctionRegistry> reg;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&reg).ok());

  {
    auto node = std::unique_ptr<ExecNode>(new ConstantValueNode(std::string{}));
    ExecEngine engine(DumpIROpt());
    ASSERT_TRUE(engine.Compile(node, reg).ok());
    EXPECT_FALSE(HasNonzeroInttoptr(std::string(engine.GetIRCode())));
    RetType result;
    ASSERT_TRUE(engine.Execute(nullptr, &result).ok());
    EXPECT_EQ(std::get<std::string>(result), "");
  }
  {
    auto node = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
    ExecEngine engine(DumpIROpt());
    ASSERT_TRUE(engine.Compile(node, reg).ok());
    EXPECT_FALSE(HasNonzeroInttoptr(std::string(engine.GetIRCode())));
    RetType result;
    ASSERT_TRUE(engine.Execute(nullptr, &result).ok());
    EXPECT_EQ(std::get<std::vector<int32_t>>(result), std::vector<int32_t>{});
  }
  {
    auto node = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<std::string>{}));
    ExecEngine engine(DumpIROpt());
    ASSERT_TRUE(engine.Compile(node, reg).ok());
    EXPECT_FALSE(HasNonzeroInttoptr(std::string(engine.GetIRCode())));
    RetType result;
    ASSERT_TRUE(engine.Execute(nullptr, &result).ok());
    EXPECT_EQ(std::get<std::vector<std::string>>(result), std::vector<std::string>{});
  }
}

TEST(RelocatableConstTest, DuplicateLiteralIsDeduped) {
  // Two identical string literals in the same Module should share the
  // same underlying payload global (const_string_cache hit). We can't
  // directly inspect the LLVM module from here, but we can check that
  // the IR still only contains a single "jf.str" style global for the
  // repeated payload — i.e. the dedup cache is actually used.
  std::unique_ptr<FunctionRegistry> reg;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&reg).ok());

  std::vector<std::string> data = {"repeat_me", "repeat_me", "repeat_me"};
  auto node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));

  ExecEngine engine(DumpIROpt());
  ASSERT_TRUE(engine.Compile(node, reg).ok());

  std::string ir(engine.GetIRCode());
  ASSERT_FALSE(ir.empty());
  // The payload "repeat_me" is 9 bytes; after dedup there should be
  // exactly one `[9 x i8]` ConstantDataArray initializer carrying it.
  // We search for the literal byte string form that LLVM prints for
  // ConstantDataArray (`c"repeat_me"`). Post-O3, LLVM may sink this
  // into a .rodata merge but will not duplicate byte-identical
  // constants.
  std::regex re(R"(c\"repeat_me\")");
  auto begin = std::sregex_iterator(ir.begin(), ir.end(), re);
  auto end = std::sregex_iterator();
  const auto count = std::distance(begin, end);
  EXPECT_EQ(count, 1) << "Expected exactly 1 occurrence of the payload literal in IR, got " << count << ":\n" << ir;
}

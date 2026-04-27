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

namespace jitfusion {

namespace {

// Helper: run the expression tree and return the final result.
Status RunExpr(std::unique_ptr<ExecNode> expr, RetType* result) {
  std::unique_ptr<FunctionRegistry> func_registry;
  auto st = FunctionRegistryFactory::CreateFunctionRegistry(&func_registry);
  if (!st.ok()) {
    return st;
  }
  ExecEngine engine;
  st = engine.Compile(expr, func_registry);
  if (!st.ok()) {
    return st;
  }
  return engine.Execute(nullptr, result);
}

// Build: <FuncName>(<list>, exec_ctx)  for nullary-over-list ops.
std::unique_ptr<ExecNode> MakeUnaryListCall(const std::string& func_name, std::unique_ptr<ExecNode> list_node) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(list_node));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  return std::unique_ptr<ExecNode>(new FunctionNode(func_name, std::move(args)));
}

// Build: <FuncName>(<list>, <scalar>, exec_ctx)
std::unique_ptr<ExecNode> MakeListScalarCall(const std::string& func_name, std::unique_ptr<ExecNode> list_node,
                                             std::unique_ptr<ExecNode> scalar_node) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(list_node));
  args.emplace_back(std::move(scalar_node));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  return std::unique_ptr<ExecNode>(new FunctionNode(func_name, std::move(args)));
}

// Build: <FuncName>(<lhs>, <rhs>, exec_ctx)  for list-list ops.
std::unique_ptr<ExecNode> MakeListListCall(const std::string& func_name, std::unique_ptr<ExecNode> lhs,
                                           std::unique_ptr<ExecNode> rhs) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(lhs));
  args.emplace_back(std::move(rhs));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  return std::unique_ptr<ExecNode>(new FunctionNode(func_name, std::move(args)));
}

}  // namespace

// ---- list_arithmetic.cc: list op scalar ------------------------------------

TEST(EmptyListRuntimeTest, ListAddEmptyList) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  auto scalar = std::unique_ptr<ExecNode>(new ConstantValueNode(int32_t{7}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeListScalarCall("ListAdd", std::move(list), std::move(scalar)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<int32_t>>(result).empty());
}

TEST(EmptyListRuntimeTest, ListMulEmptyListFloat) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<float>{}));
  auto scalar = std::unique_ptr<ExecNode>(new ConstantValueNode(2.5F));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeListScalarCall("ListMul", std::move(list), std::move(scalar)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<float>>(result).empty());
}

TEST(EmptyListRuntimeTest, ListSubEmptyListEmptyList) {
  auto lhs = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int64_t>{}));
  auto rhs = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int64_t>{}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeListListCall("ListSub", std::move(lhs), std::move(rhs)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<int64_t>>(result).empty());
}

TEST(EmptyListRuntimeTest, ListBitwiseAndEmptyListEmptyList) {
  auto lhs = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{}));
  auto rhs = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeListListCall("ListBitwiseAnd", std::move(lhs), std::move(rhs)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<uint32_t>>(result).empty());
}

TEST(EmptyListRuntimeTest, ListExpEmptyListGoesToF64) {
  // ListExp promotes integer input to F64List.
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeUnaryListCall("ListExp", std::move(list)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<double>>(result).empty());
}

TEST(EmptyListRuntimeTest, ListMinEmptyList) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint8_t>{}));
  auto scalar = std::unique_ptr<ExecNode>(new ConstantValueNode(uint8_t{5}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeListScalarCall("ListMin", std::move(list), std::move(scalar)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<uint8_t>>(result).empty());
}

// ---- list_basic.cc ---------------------------------------------------------

TEST(EmptyListRuntimeTest, ListConcatTwoEmptyLists) {
  auto lhs = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  auto rhs = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeListListCall("ListConcat", std::move(lhs), std::move(rhs)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<int32_t>>(result).empty());
}

TEST(EmptyListRuntimeTest, ListConcatEmptyWithNonEmpty) {
  auto lhs = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  auto rhs = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeListListCall("ListConcat", std::move(lhs), std::move(rhs)), &result).ok());
  std::vector<int32_t> expected = {1, 2, 3};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

TEST(EmptyListRuntimeTest, SortAscEmptyList) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeUnaryListCall("SortAsc", std::move(list)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<int32_t>>(result).empty());
}

TEST(EmptyListRuntimeTest, SortDescEmptyListFloat) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<double>{}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeUnaryListCall("SortDesc", std::move(list)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<double>>(result).empty());
}

TEST(EmptyListRuntimeTest, UniqueEmptyList) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint64_t>{}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeUnaryListCall("Unique", std::move(list)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<uint64_t>>(result).empty());
}

TEST(EmptyListRuntimeTest, UniqueEmptyStringList) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<std::string>{}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeUnaryListCall("Unique", std::move(list)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<std::string>>(result).empty());
}

TEST(EmptyListRuntimeTest, CastEmptyI32ListToI64List) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeUnaryListCall("CastI64List", std::move(list)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<int64_t>>(result).empty());
}

TEST(EmptyListRuntimeTest, CastEmptyI32ListToStringList) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeUnaryListCall("CastStringList", std::move(list)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<std::string>>(result).empty());
}

TEST(EmptyListRuntimeTest, CastEmptyStringListToI32List) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<std::string>{}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeUnaryListCall("CastI32List", std::move(list)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<int32_t>>(result).empty());
}

// ---- list_comparison.cc: GenBitmap family (the one that had the real bug) --

TEST(EmptyListRuntimeTest, GenLargeBitmapEmptyListScalar) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  auto scalar = std::unique_ptr<ExecNode>(new ConstantValueNode(int32_t{0}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeListScalarCall("GenLargeBitmap", std::move(list), std::move(scalar)), &result).ok());
  // (0 + 7) / 8 == 0: empty bitmap.
  EXPECT_TRUE(std::get<std::vector<uint8_t>>(result).empty());
}

TEST(EmptyListRuntimeTest, GenEqualBitmapEmptyListEmptyList) {
  auto lhs = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{}));
  auto rhs = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeListListCall("GenEqualBitmap", std::move(lhs), std::move(rhs)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<uint8_t>>(result).empty());
}

TEST(EmptyListRuntimeTest, GenLessBitmapEmptyListFloat) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<double>{}));
  auto scalar = std::unique_ptr<ExecNode>(new ConstantValueNode(0.0));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeListScalarCall("GenLessBitmap", std::move(list), std::move(scalar)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<uint8_t>>(result).empty());
}

TEST(EmptyListRuntimeTest, GenNotEqualBitmapListEmptyVsEmpty) {
  auto lhs = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int64_t>{}));
  auto rhs = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int64_t>{}));
  RetType result;
  ASSERT_TRUE(RunExpr(MakeListListCall("GenNotEqualBitmap", std::move(lhs), std::move(rhs)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<uint8_t>>(result).empty());
}

// ---- list_comparison.cc: IfXxx on empty list -------------------------------

TEST(EmptyListRuntimeTest, IfLargeOnEmptyList) {
  // IfLarge(list, cmp_value, target_value, exec_ctx)
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  auto cmp = std::unique_ptr<ExecNode>(new ConstantValueNode(int32_t{5}));
  auto target = std::unique_ptr<ExecNode>(new ConstantValueNode(int32_t{99}));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(list));
  args.emplace_back(std::move(cmp));
  args.emplace_back(std::move(target));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  auto expr = std::unique_ptr<ExecNode>(new FunctionNode("IfLarge", std::move(args)));
  RetType result;
  ASSERT_TRUE(RunExpr(std::move(expr), &result).ok());
  EXPECT_TRUE(std::get<std::vector<int32_t>>(result).empty());
}

// ---- string_function_init.cc -----------------------------------------------

TEST(EmptyListRuntimeTest, StringConcatTwoEmptyStrings) {
  auto a = std::unique_ptr<ExecNode>(new ConstantValueNode(std::string{}));
  auto b = std::unique_ptr<ExecNode>(new ConstantValueNode(std::string{}));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(a));
  args.emplace_back(std::move(b));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  auto expr = std::unique_ptr<ExecNode>(new FunctionNode("StringConcat", std::move(args)));
  RetType result;
  ASSERT_TRUE(RunExpr(std::move(expr), &result).ok());
  EXPECT_TRUE(std::get<std::string>(result).empty());
}

TEST(EmptyListRuntimeTest, StringConcatEmptyWithNonEmpty) {
  auto a = std::unique_ptr<ExecNode>(new ConstantValueNode(std::string{}));
  auto b = std::unique_ptr<ExecNode>(new ConstantValueNode(std::string{"hello"}));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(a));
  args.emplace_back(std::move(b));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  auto expr = std::unique_ptr<ExecNode>(new FunctionNode("StringConcat", std::move(args)));
  RetType result;
  ASSERT_TRUE(RunExpr(std::move(expr), &result).ok());
  EXPECT_EQ(std::get<std::string>(result), "hello");
}

}  // namespace jitfusion

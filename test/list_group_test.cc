/*
 * @Author: victorika
 * @Date: 2026-04-30 11:35:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-30 11:35:00
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

namespace jitfusion {

namespace {

std::unique_ptr<ExecNode> MakeGroupIndexCall(std::unique_ptr<ExecNode> list_node) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(list_node));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  return std::unique_ptr<ExecNode>(new FunctionNode("GroupIndex", std::move(args)));
}

Status RunGroupIndex(std::unique_ptr<ExecNode> expr, RetType *result) {
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

}  // namespace

TEST(ListGroupTest, GroupIndexEmptyI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<uint32_t>>(result).empty());
}

TEST(ListGroupTest, GroupIndexAllSameI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{7, 7, 7, 7}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 0, 0, 0};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexAllDistinctI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 30, 40, 50}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 1, 2, 3, 4};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexInterleavedI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{3, 1, 4, 1, 5, 3, 5, 3}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 1, 2, 1, 3, 0, 3, 0};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexSingleElementI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{42}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexU8) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint8_t>{2, 2, 5, 2, 5, 9}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 0, 1, 0, 1, 2};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexI8Negatives) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int8_t>{-1, -2, -1, -3, -2}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 1, 0, 2, 1};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexU16) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint16_t>{100, 200, 100, 300}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 1, 0, 2};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexI16) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int16_t>{-1000, 1000, -1000}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 1, 0};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexU32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{1U, 1U, 2U}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 0, 1};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexU64Large) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(
      std::vector<uint64_t>{0xFFFFFFFFFFFFFFFFULL, 1ULL, 0xFFFFFFFFFFFFFFFFULL, 1ULL, 2ULL}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 1, 0, 1, 2};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexI64) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int64_t>{-9LL, 9LL, -9LL, 0LL}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 1, 0, 2};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexF32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<float>{1.5F, 2.5F, 1.5F, 3.5F, 2.5F}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 1, 0, 2, 1};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexF64) {
  auto list =
      std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<double>{3.14, 2.72, 3.14, 1.41, 2.72, 3.14}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 1, 0, 2, 1, 0};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexString) {
  auto list = std::unique_ptr<ExecNode>(
      new ConstantListValueNode(std::vector<std::string>{"apple", "banana", "apple", "cherry", "banana", "apple"}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 1, 0, 2, 1, 0};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexStringEmpty) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<std::string>{}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<uint32_t>>(result).empty());
}

TEST(ListGroupTest, GroupIndexStringWithEmptyStrings) {
  auto list = std::unique_ptr<ExecNode>(
      new ConstantListValueNode(std::vector<std::string>{"", "a", "", "b", "a", ""}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 1, 0, 2, 1, 0};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexStringDifferentLengths) {
  auto list = std::unique_ptr<ExecNode>(
      new ConstantListValueNode(std::vector<std::string>{"ab", "abc", "ab", "abcd", "abc"}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {0, 1, 0, 2, 1};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupIndexResultLenMatchesInputLen) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{5, 5, 5, 5, 5, 5, 5}));
  RetType result;
  ASSERT_TRUE(RunGroupIndex(MakeGroupIndexCall(std::move(list)), &result).ok());
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result).size(), 7U);
}

namespace {

// Builds: GroupCount(GroupIndex(keys)). GroupCount is a pure readonly
// computation so no ExecContext argument is needed.
std::unique_ptr<ExecNode> MakeGroupCountCall(std::unique_ptr<ExecNode> keys_node) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(MakeGroupIndexCall(std::move(keys_node)));
  return std::unique_ptr<ExecNode>(new FunctionNode("GroupCount", std::move(args)));
}

Status RunGroupCount(std::unique_ptr<ExecNode> expr, RetType *result) {
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

}  // namespace

TEST(ListGroupTest, GroupCountEmptyI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  RetType result;
  ASSERT_TRUE(RunGroupCount(MakeGroupCountCall(std::move(list)), &result).ok());
  EXPECT_EQ(std::get<uint32_t>(result), 0U);
}

TEST(ListGroupTest, GroupCountSingleElementI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{42}));
  RetType result;
  ASSERT_TRUE(RunGroupCount(MakeGroupCountCall(std::move(list)), &result).ok());
  EXPECT_EQ(std::get<uint32_t>(result), 1U);
}

TEST(ListGroupTest, GroupCountAllSameI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{7, 7, 7, 7}));
  RetType result;
  ASSERT_TRUE(RunGroupCount(MakeGroupCountCall(std::move(list)), &result).ok());
  EXPECT_EQ(std::get<uint32_t>(result), 1U);
}

TEST(ListGroupTest, GroupCountAllDistinctI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 30, 40, 50}));
  RetType result;
  ASSERT_TRUE(RunGroupCount(MakeGroupCountCall(std::move(list)), &result).ok());
  EXPECT_EQ(std::get<uint32_t>(result), 5U);
}

TEST(ListGroupTest, GroupCountInterleavedI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{3, 1, 4, 1, 5, 3, 5, 3}));
  RetType result;
  ASSERT_TRUE(RunGroupCount(MakeGroupCountCall(std::move(list)), &result).ok());
  EXPECT_EQ(std::get<uint32_t>(result), 4U);
}

TEST(ListGroupTest, GroupCountString) {
  auto list = std::unique_ptr<ExecNode>(
      new ConstantListValueNode(std::vector<std::string>{"apple", "banana", "apple", "cherry", "banana", "apple"}));
  RetType result;
  ASSERT_TRUE(RunGroupCount(MakeGroupCountCall(std::move(list)), &result).ok());
  EXPECT_EQ(std::get<uint32_t>(result), 3U);
}

namespace {

// Builds: GroupKeys(keys, GroupIndex(keys), GroupCount(GroupIndex(keys)), exec_ctx).
// The keys node is cloned so the same logical list feeds every argument,
// mirroring how CSE would share the GroupIndex / GroupCount sub-expressions
// with other aggregates in a real pipeline.
std::unique_ptr<ExecNode> MakeGroupKeysCall(std::unique_ptr<ExecNode> keys_node) {
  auto keys_for_index = keys_node->Clone();
  auto keys_for_count = keys_node->Clone();
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(keys_node));
  args.emplace_back(MakeGroupIndexCall(std::move(keys_for_index)));
  args.emplace_back(MakeGroupCountCall(std::move(keys_for_count)));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  return std::unique_ptr<ExecNode>(new FunctionNode("GroupKeys", std::move(args)));
}

Status RunGroupKeys(std::unique_ptr<ExecNode> expr, RetType *result) {
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

}  // namespace

TEST(ListGroupTest, GroupKeysEmptyI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<int32_t>>(result).empty());
}

TEST(ListGroupTest, GroupKeysSingleElementI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{42}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<int32_t> expected = {42};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysAllSameI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{7, 7, 7, 7}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<int32_t> expected = {7};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysAllDistinctI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 30, 40, 50}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<int32_t> expected = {10, 20, 30, 40, 50};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysInterleavedI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{3, 1, 4, 1, 5, 3, 5, 3}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<int32_t> expected = {3, 1, 4, 5};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysU8) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint8_t>{2, 2, 5, 2, 5, 9}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<uint8_t> expected = {2, 5, 9};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysI8Negatives) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int8_t>{-1, -2, -1, -3, -2}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<int8_t> expected = {-1, -2, -3};
  EXPECT_EQ(std::get<std::vector<int8_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysU16) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint16_t>{100, 200, 100, 300}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<uint16_t> expected = {100, 200, 300};
  EXPECT_EQ(std::get<std::vector<uint16_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysI16) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int16_t>{-1000, 1000, -1000}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<int16_t> expected = {-1000, 1000};
  EXPECT_EQ(std::get<std::vector<int16_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysU32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{1U, 1U, 2U}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {1U, 2U};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysU64Large) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(
      std::vector<uint64_t>{0xFFFFFFFFFFFFFFFFULL, 1ULL, 0xFFFFFFFFFFFFFFFFULL, 1ULL, 2ULL}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<uint64_t> expected = {0xFFFFFFFFFFFFFFFFULL, 1ULL, 2ULL};
  EXPECT_EQ(std::get<std::vector<uint64_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysI64) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int64_t>{-9LL, 9LL, -9LL, 0LL}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<int64_t> expected = {-9LL, 9LL, 0LL};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysF32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<float>{1.5F, 2.5F, 1.5F, 3.5F, 2.5F}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<float> expected = {1.5F, 2.5F, 3.5F};
  EXPECT_EQ(std::get<std::vector<float>>(result), expected);
}

TEST(ListGroupTest, GroupKeysF64) {
  auto list =
      std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<double>{3.14, 2.72, 3.14, 1.41, 2.72, 3.14}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<double> expected = {3.14, 2.72, 1.41};
  EXPECT_EQ(std::get<std::vector<double>>(result), expected);
}

TEST(ListGroupTest, GroupKeysString) {
  auto list = std::unique_ptr<ExecNode>(
      new ConstantListValueNode(std::vector<std::string>{"apple", "banana", "apple", "cherry", "banana", "apple"}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<std::string> expected = {"apple", "banana", "cherry"};
  EXPECT_EQ(std::get<std::vector<std::string>>(result), expected);
}

TEST(ListGroupTest, GroupKeysStringEmpty) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<std::string>{}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<std::string>>(result).empty());
}

TEST(ListGroupTest, GroupKeysStringWithEmptyStrings) {
  auto list = std::unique_ptr<ExecNode>(
      new ConstantListValueNode(std::vector<std::string>{"", "a", "", "b", "a", ""}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<std::string> expected = {"", "a", "b"};
  EXPECT_EQ(std::get<std::vector<std::string>>(result), expected);
}

TEST(ListGroupTest, GroupKeysStringDifferentLengths) {
  auto list = std::unique_ptr<ExecNode>(
      new ConstantListValueNode(std::vector<std::string>{"ab", "abc", "ab", "abcd", "abc"}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<std::string> expected = {"ab", "abc", "abcd"};
  EXPECT_EQ(std::get<std::vector<std::string>>(result), expected);
}

TEST(ListGroupTest, GroupKeysPreservesFirstAppearanceOrder) {
  // Distinct keys must come out in the order of their first occurrence, not
  // sorted and not in hash-table order.
  auto list = std::unique_ptr<ExecNode>(
      new ConstantListValueNode(std::vector<int32_t>{100, 50, 100, 75, 50, 25, 75, 100}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysCall(std::move(list)), &result).ok());
  std::vector<int32_t> expected = {100, 50, 75, 25};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysLenMismatchReturnsError) {
  // keys has 4 elements, group_index has 3 -> must surface a runtime error
  // instead of silently truncating.
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 1, 3}));
  auto bad_group_index =
      std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{0U, 1U, 0U}));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(keys));
  args.emplace_back(std::move(bad_group_index));
  args.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint32_t>(2U))));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  auto expr = std::unique_ptr<ExecNode>(new FunctionNode("GroupKeys", std::move(args)));

  RetType result;
  Status st = RunGroupKeys(std::move(expr), &result);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("GroupKeys"), std::string::npos);
  EXPECT_NE(st.ToString().find("len mismatch"), std::string::npos);
}

namespace {

// Builds the 3-arg sugar form: GroupKeys(keys, GroupIndex(keys), exec_ctx).
// This is the user-facing call registered as an LLVM intrinsic that expands
// at IR time into GroupCount + the canonical 4-arg GroupKeys kernel, letting
// GVN share the distinct-count computation with other aggregates. The
// trailing exec_context keeps the call shape consistent with other DSL
// functions (GroupIndex, etc.).
std::unique_ptr<ExecNode> MakeGroupKeysSugarCall(std::unique_ptr<ExecNode> keys_node) {
  auto keys_for_index = keys_node->Clone();
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(keys_node));
  args.emplace_back(MakeGroupIndexCall(std::move(keys_for_index)));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  return std::unique_ptr<ExecNode>(new FunctionNode("GroupKeys", std::move(args)));
}

}  // namespace

TEST(ListGroupTest, GroupKeysSugarEmptyI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<int32_t>>(result).empty());
}

TEST(ListGroupTest, GroupKeysSugarSingleElementI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{42}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  std::vector<int32_t> expected = {42};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysSugarAllSameI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{7, 7, 7, 7}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  std::vector<int32_t> expected = {7};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysSugarAllDistinctI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 30, 40, 50}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  std::vector<int32_t> expected = {10, 20, 30, 40, 50};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysSugarInterleavedI32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{3, 1, 4, 1, 5, 3, 5, 3}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  std::vector<int32_t> expected = {3, 1, 4, 5};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysSugarU8) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint8_t>{2, 2, 5, 2, 5, 9}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  std::vector<uint8_t> expected = {2, 5, 9};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysSugarI8Negatives) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int8_t>{-1, -2, -1, -3, -2}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  std::vector<int8_t> expected = {-1, -2, -3};
  EXPECT_EQ(std::get<std::vector<int8_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysSugarU16) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint16_t>{100, 200, 100, 300}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  std::vector<uint16_t> expected = {100, 200, 300};
  EXPECT_EQ(std::get<std::vector<uint16_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysSugarI16) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int16_t>{-1000, 1000, -1000}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  std::vector<int16_t> expected = {-1000, 1000};
  EXPECT_EQ(std::get<std::vector<int16_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysSugarU32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{1U, 1U, 2U}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  std::vector<uint32_t> expected = {1U, 2U};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysSugarU64Large) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(
      std::vector<uint64_t>{0xFFFFFFFFFFFFFFFFULL, 1ULL, 0xFFFFFFFFFFFFFFFFULL, 1ULL, 2ULL}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  std::vector<uint64_t> expected = {0xFFFFFFFFFFFFFFFFULL, 1ULL, 2ULL};
  EXPECT_EQ(std::get<std::vector<uint64_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysSugarI64) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int64_t>{-9LL, 9LL, -9LL, 0LL}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  std::vector<int64_t> expected = {-9LL, 9LL, 0LL};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expected);
}

TEST(ListGroupTest, GroupKeysSugarF32) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<float>{1.5F, 2.5F, 1.5F, 3.5F, 2.5F}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  std::vector<float> expected = {1.5F, 2.5F, 3.5F};
  EXPECT_EQ(std::get<std::vector<float>>(result), expected);
}

TEST(ListGroupTest, GroupKeysSugarF64) {
  auto list =
      std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<double>{3.14, 2.72, 3.14, 1.41, 2.72, 3.14}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  std::vector<double> expected = {3.14, 2.72, 1.41};
  EXPECT_EQ(std::get<std::vector<double>>(result), expected);
}

TEST(ListGroupTest, GroupKeysSugarString) {
  auto list = std::unique_ptr<ExecNode>(
      new ConstantListValueNode(std::vector<std::string>{"apple", "banana", "apple", "cherry", "banana", "apple"}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  std::vector<std::string> expected = {"apple", "banana", "cherry"};
  EXPECT_EQ(std::get<std::vector<std::string>>(result), expected);
}

TEST(ListGroupTest, GroupKeysSugarStringEmpty) {
  auto list = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<std::string>{}));
  RetType result;
  ASSERT_TRUE(RunGroupKeys(MakeGroupKeysSugarCall(std::move(list)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<std::string>>(result).empty());
}

TEST(ListGroupTest, GroupKeysSugarLenMismatchReturnsError) {
  // The sugar form forwards exec_context to the 4-arg kernel, which must still
  // surface the same "len mismatch" runtime error path.
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 1, 3}));
  auto bad_group_index =
      std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{0U, 1U, 0U}));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(keys));
  args.emplace_back(std::move(bad_group_index));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  auto expr = std::unique_ptr<ExecNode>(new FunctionNode("GroupKeys", std::move(args)));

  RetType result;
  Status st = RunGroupKeys(std::move(expr), &result);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("GroupKeys"), std::string::npos);
  EXPECT_NE(st.ToString().find("len mismatch"), std::string::npos);
}

namespace {

// Builds: GroupSum(values, GroupIndex(keys), exec_ctx). Sugar form, same call
// shape as the user-facing DSL -- the IR-level expansion adds GroupCount and
// routes to the 4-arg kernel. Keys and values are separate nodes because they
// may be of different element types.
std::unique_ptr<ExecNode> MakeGroupSumSugarCall(std::unique_ptr<ExecNode> values_node,
                                                std::unique_ptr<ExecNode> keys_node) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(values_node));
  args.emplace_back(MakeGroupIndexCall(std::move(keys_node)));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  return std::unique_ptr<ExecNode>(new FunctionNode("GroupSum", std::move(args)));
}

// Builds: GroupSum(values, GroupIndex(keys), GroupCount(GroupIndex(keys)), exec_ctx).
// Exercises the raw 4-arg kernel directly so its contract is covered
// independently of the sugar-layer expansion.
std::unique_ptr<ExecNode> MakeGroupSumKernelCall(std::unique_ptr<ExecNode> values_node,
                                                 std::unique_ptr<ExecNode> keys_node) {
  auto keys_for_count = keys_node->Clone();
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(values_node));
  args.emplace_back(MakeGroupIndexCall(std::move(keys_node)));
  args.emplace_back(MakeGroupCountCall(std::move(keys_for_count)));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  return std::unique_ptr<ExecNode>(new FunctionNode("GroupSum", std::move(args)));
}

Status RunGroupSum(std::unique_ptr<ExecNode> expr, RetType *result) {
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

}  // namespace

// ---- GroupSum sugar form tests ---------------------------------------------

TEST(ListGroupTest, GroupSumSugarI32) {
  // keys:   {10, 20, 10, 30, 20, 10}  -> gids: {0, 1, 0, 2, 1, 0}
  // values: {1,  2,  3,  4,  5,  6}   -> groups: [1+3+6=10, 2+5=7, 4]
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 10, 30, 20, 10}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3, 4, 5, 6}));
  RetType result;
  ASSERT_TRUE(RunGroupSum(MakeGroupSumSugarCall(std::move(values), std::move(keys)), &result).ok());
  std::vector<int64_t> expected = {10, 7, 4};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expected);
}

TEST(ListGroupTest, GroupSumSugarI8NegativeValues) {
  // Exercise i8 -> i64 promotion with negative values.
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 1, 2, 1, 2}));
  auto values =
      std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int8_t>{-10, 20, -30, -40, 50}));
  RetType result;
  ASSERT_TRUE(RunGroupSum(MakeGroupSumSugarCall(std::move(values), std::move(keys)), &result).ok());
  // group 0: -10 + 20 + (-40) = -30; group 1: -30 + 50 = 20
  std::vector<int64_t> expected = {-30, 20};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expected);
}

TEST(ListGroupTest, GroupSumSugarI16) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 1, 2}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int16_t>{1000, -1000, 2000, 3000}));
  RetType result;
  ASSERT_TRUE(RunGroupSum(MakeGroupSumSugarCall(std::move(values), std::move(keys)), &result).ok());
  std::vector<int64_t> expected = {3000, 2000};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expected);
}

TEST(ListGroupTest, GroupSumSugarI64) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 1}));
  auto values = std::unique_ptr<ExecNode>(
      new ConstantListValueNode(std::vector<int64_t>{1LL << 40, 1LL << 41, -(1LL << 40)}));
  RetType result;
  ASSERT_TRUE(RunGroupSum(MakeGroupSumSugarCall(std::move(values), std::move(keys)), &result).ok());
  std::vector<int64_t> expected = {0, 1LL << 41};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expected);
}

TEST(ListGroupTest, GroupSumSugarU8) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 1, 0, 2, 1}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint8_t>{250, 200, 100, 50, 150}));
  RetType result;
  ASSERT_TRUE(RunGroupSum(MakeGroupSumSugarCall(std::move(values), std::move(keys)), &result).ok());
  // u8 promotes to u64, so 250 + 100 does not overflow.
  std::vector<uint64_t> expected = {350U, 350U, 50U};
  EXPECT_EQ(std::get<std::vector<uint64_t>>(result), expected);
}

TEST(ListGroupTest, GroupSumSugarU64Large) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 0, 1}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(
      std::vector<uint64_t>{0xFFFFFFFFFFFFFFFEULL, 1ULL, 42ULL}));
  RetType result;
  ASSERT_TRUE(RunGroupSum(MakeGroupSumSugarCall(std::move(values), std::move(keys)), &result).ok());
  // Group 0 sums exactly to UINT64_MAX.
  std::vector<uint64_t> expected = {0xFFFFFFFFFFFFFFFFULL, 42ULL};
  EXPECT_EQ(std::get<std::vector<uint64_t>>(result), expected);
}

TEST(ListGroupTest, GroupSumSugarF32) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 1, 0, 1}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<float>{1.5F, 2.5F, 3.5F, 4.5F}));
  RetType result;
  ASSERT_TRUE(RunGroupSum(MakeGroupSumSugarCall(std::move(values), std::move(keys)), &result).ok());
  std::vector<double> expected = {5.0, 7.0};
  auto &actual = std::get<std::vector<double>>(result);
  ASSERT_EQ(actual.size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_DOUBLE_EQ(actual[i], expected[i]);
  }
}

TEST(ListGroupTest, GroupSumSugarF64) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 1, 0, 1, 2}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<double>{0.1, 0.2, 0.3, 0.4, 0.5}));
  RetType result;
  ASSERT_TRUE(RunGroupSum(MakeGroupSumSugarCall(std::move(values), std::move(keys)), &result).ok());
  std::vector<double> expected = {0.4, 0.6, 0.5};
  auto &actual = std::get<std::vector<double>>(result);
  ASSERT_EQ(actual.size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_DOUBLE_EQ(actual[i], expected[i]);
  }
}

TEST(ListGroupTest, GroupSumSugarEmpty) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  RetType result;
  ASSERT_TRUE(RunGroupSum(MakeGroupSumSugarCall(std::move(values), std::move(keys)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<int64_t>>(result).empty());
}

TEST(ListGroupTest, GroupSumSugarAllSameGroup) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{7, 7, 7, 7}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3, 4}));
  RetType result;
  ASSERT_TRUE(RunGroupSum(MakeGroupSumSugarCall(std::move(values), std::move(keys)), &result).ok());
  std::vector<int64_t> expected = {10};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expected);
}

TEST(ListGroupTest, GroupSumSugarAllDistinctGroups) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 30, 40}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3, 4}));
  RetType result;
  ASSERT_TRUE(RunGroupSum(MakeGroupSumSugarCall(std::move(values), std::move(keys)), &result).ok());
  std::vector<int64_t> expected = {1, 2, 3, 4};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expected);
}

TEST(ListGroupTest, GroupSumSugarStringKeys) {
  // keys are strings, values are numeric. GroupIndex handles the string keys;
  // GroupSum just operates on the resulting u32 group_index, so this exercises
  // the full "keys can be anything, values can be anything" combination.
  auto keys = std::unique_ptr<ExecNode>(
      new ConstantListValueNode(std::vector<std::string>{"a", "b", "a", "c", "b", "a"}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3, 4, 5, 6}));
  RetType result;
  ASSERT_TRUE(RunGroupSum(MakeGroupSumSugarCall(std::move(values), std::move(keys)), &result).ok());
  // "a": 1+3+6=10, "b": 2+5=7, "c": 4
  std::vector<int64_t> expected = {10, 7, 4};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expected);
}

// ---- GroupSum kernel (4-arg) tests -----------------------------------------

TEST(ListGroupTest, GroupSumKernelI32) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 10, 30, 20, 10}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3, 4, 5, 6}));
  RetType result;
  ASSERT_TRUE(RunGroupSum(MakeGroupSumKernelCall(std::move(values), std::move(keys)), &result).ok());
  std::vector<int64_t> expected = {10, 7, 4};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expected);
}

TEST(ListGroupTest, GroupSumKernelU32) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 1, 0}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{100U, 200U, 300U}));
  RetType result;
  ASSERT_TRUE(RunGroupSum(MakeGroupSumKernelCall(std::move(values), std::move(keys)), &result).ok());
  std::vector<uint64_t> expected = {400U, 200U};
  EXPECT_EQ(std::get<std::vector<uint64_t>>(result), expected);
}

TEST(ListGroupTest, GroupSumKernelSugarAgreeF64) {
  // Direct equivalence check between the sugar and the kernel on the same
  // inputs -- protects against future divergence (e.g. if sugar starts
  // passing a different `distinct`).
  auto keys_sugar = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{2, 2, 1, 2, 1}));
  auto values_sugar = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<double>{1.1, 2.2, 3.3, 4.4, 5.5}));
  RetType sugar_result;
  ASSERT_TRUE(
      RunGroupSum(MakeGroupSumSugarCall(std::move(values_sugar), std::move(keys_sugar)), &sugar_result).ok());

  auto keys_kernel = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{2, 2, 1, 2, 1}));
  auto values_kernel =
      std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<double>{1.1, 2.2, 3.3, 4.4, 5.5}));
  RetType kernel_result;
  ASSERT_TRUE(
      RunGroupSum(MakeGroupSumKernelCall(std::move(values_kernel), std::move(keys_kernel)), &kernel_result).ok());

  auto &sugar_vec = std::get<std::vector<double>>(sugar_result);
  auto &kernel_vec = std::get<std::vector<double>>(kernel_result);
  ASSERT_EQ(sugar_vec.size(), kernel_vec.size());
  for (size_t i = 0; i < sugar_vec.size(); ++i) {
    EXPECT_DOUBLE_EQ(sugar_vec[i], kernel_vec[i]);
  }
}

// ---- GroupSum error paths --------------------------------------------------

TEST(ListGroupTest, GroupSumLenMismatchReturnsError) {
  // values has 3 elements, group_index has 4 -> kernel must surface a runtime
  // error, same shape as GroupKeys len-mismatch handling.
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3}));
  auto bad_group_index =
      std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{0U, 1U, 0U, 1U}));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(values));
  args.emplace_back(std::move(bad_group_index));
  args.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint32_t>(2U))));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  auto expr = std::unique_ptr<ExecNode>(new FunctionNode("GroupSum", std::move(args)));

  RetType result;
  Status st = RunGroupSum(std::move(expr), &result);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("GroupSum"), std::string::npos);
  EXPECT_NE(st.ToString().find("len mismatch"), std::string::npos);
}

TEST(ListGroupTest, GroupSumSugarLenMismatchReturnsError) {
  // Same kind of mismatch reached through the sugar form, to prove the error
  // propagates across the IR-level expansion.
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3}));
  auto bad_group_index =
      std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{0U, 1U, 0U, 1U}));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(values));
  args.emplace_back(std::move(bad_group_index));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  auto expr = std::unique_ptr<ExecNode>(new FunctionNode("GroupSum", std::move(args)));

  RetType result;
  Status st = RunGroupSum(std::move(expr), &result);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("GroupSum"), std::string::npos);
  EXPECT_NE(st.ToString().find("len mismatch"), std::string::npos);
}

namespace {

// Generic builder for a per-group aggregate sugar call of shape
// `<func_name>(values, GroupIndex(keys), exec_ctx)`. Used by GroupMax / GroupMin
// tests and ready to be reused by later aggregates (Avg, ...).
std::unique_ptr<ExecNode> MakeGroupAggSugarCall(const std::string &func_name, std::unique_ptr<ExecNode> values_node,
                                                std::unique_ptr<ExecNode> keys_node) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(values_node));
  args.emplace_back(MakeGroupIndexCall(std::move(keys_node)));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  return std::unique_ptr<ExecNode>(new FunctionNode(func_name, std::move(args)));
}

// Same as above but for the raw 4-arg kernel:
//   `<func_name>(values, GroupIndex(keys), GroupCount(GroupIndex(keys)), exec_ctx)`
std::unique_ptr<ExecNode> MakeGroupAggKernelCall(const std::string &func_name, std::unique_ptr<ExecNode> values_node,
                                                 std::unique_ptr<ExecNode> keys_node) {
  auto keys_for_count = keys_node->Clone();
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(values_node));
  args.emplace_back(MakeGroupIndexCall(std::move(keys_node)));
  args.emplace_back(MakeGroupCountCall(std::move(keys_for_count)));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  return std::unique_ptr<ExecNode>(new FunctionNode(func_name, std::move(args)));
}

Status RunGroupAgg(std::unique_ptr<ExecNode> expr, RetType *result) {
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

}  // namespace

// ---- GroupMax sugar form tests ---------------------------------------------

TEST(ListGroupTest, GroupMaxSugarI32) {
  // keys:   {10, 20, 10, 30, 20, 10}  -> gids: {0, 1, 0, 2, 1, 0}
  // values: {1,  5,  3,  4,  2,  6}   -> maxes per group: {6, 5, 4}
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 10, 30, 20, 10}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 5, 3, 4, 2, 6}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupMax", std::move(values), std::move(keys)), &result).ok());
  std::vector<int32_t> expected = {6, 5, 4};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

TEST(ListGroupTest, GroupMaxSugarI8AllNegative) {
  // All-negative values must still pick the max (closest to zero), which
  // only works correctly if the seed is numeric_limits<i8>::lowest() (-128),
  // not 0.
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 0, 1, 1}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int8_t>{-30, -10, -100, -50}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupMax", std::move(values), std::move(keys)), &result).ok());
  std::vector<int8_t> expected = {-10, -50};
  EXPECT_EQ(std::get<std::vector<int8_t>>(result), expected);
}

TEST(ListGroupTest, GroupMaxSugarU8) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 0, 1, 1, 1}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint8_t>{10, 250, 0, 128, 64}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupMax", std::move(values), std::move(keys)), &result).ok());
  std::vector<uint8_t> expected = {250, 128};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expected);
}

TEST(ListGroupTest, GroupMaxSugarF64) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 1, 0, 1}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<double>{-1.5, 2.5, -3.5, 0.5}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupMax", std::move(values), std::move(keys)), &result).ok());
  auto &actual = std::get<std::vector<double>>(result);
  ASSERT_EQ(actual.size(), 2U);
  EXPECT_DOUBLE_EQ(actual[0], -1.5);
  EXPECT_DOUBLE_EQ(actual[1], 2.5);
}

TEST(ListGroupTest, GroupMaxSugarSingleton) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{42}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{-7}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupMax", std::move(values), std::move(keys)), &result).ok());
  std::vector<int32_t> expected = {-7};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

TEST(ListGroupTest, GroupMaxSugarEmpty) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupMax", std::move(values), std::move(keys)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<int32_t>>(result).empty());
}

TEST(ListGroupTest, GroupMaxSugarStringKeys) {
  auto keys = std::unique_ptr<ExecNode>(
      new ConstantListValueNode(std::vector<std::string>{"a", "b", "a", "c", "b", "a"}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 7, 5, 4, 2, 3}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupMax", std::move(values), std::move(keys)), &result).ok());
  // "a": max(1,5,3)=5, "b": max(7,2)=7, "c": 4
  std::vector<int32_t> expected = {5, 7, 4};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

// ---- GroupMax kernel (4-arg) tests -----------------------------------------

TEST(ListGroupTest, GroupMaxKernelI32) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 10, 30, 20, 10}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 5, 3, 4, 2, 6}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggKernelCall("GroupMax", std::move(values), std::move(keys)), &result).ok());
  std::vector<int32_t> expected = {6, 5, 4};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

// ---- GroupMin sugar form tests ---------------------------------------------

TEST(ListGroupTest, GroupMinSugarI32) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 10, 30, 20, 10}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 5, 3, 4, 2, 6}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupMin", std::move(values), std::move(keys)), &result).ok());
  std::vector<int32_t> expected = {1, 2, 4};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

TEST(ListGroupTest, GroupMinSugarI8AllPositive) {
  // All-positive values must still pick the smallest, which only works
  // correctly if the seed is numeric_limits<i8>::max() (127), not 0.
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 0, 1, 1}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int8_t>{30, 10, 100, 50}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupMin", std::move(values), std::move(keys)), &result).ok());
  std::vector<int8_t> expected = {10, 50};
  EXPECT_EQ(std::get<std::vector<int8_t>>(result), expected);
}

TEST(ListGroupTest, GroupMinSugarU64Large) {
  // Use values close to UINT64_MAX to prove the seed is max(), which is the
  // only value they can all be less-or-equal to.
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 0, 1}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(
      std::vector<uint64_t>{0xFFFFFFFFFFFFFFF0ULL, 0xFFFFFFFFFFFFFFFEULL, 0xFFFFFFFFFFFFFFFFULL}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupMin", std::move(values), std::move(keys)), &result).ok());
  std::vector<uint64_t> expected = {0xFFFFFFFFFFFFFFF0ULL, 0xFFFFFFFFFFFFFFFFULL};
  EXPECT_EQ(std::get<std::vector<uint64_t>>(result), expected);
}

TEST(ListGroupTest, GroupMinSugarF64) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 1, 0, 1}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<double>{1.5, 2.5, -3.5, 0.5}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupMin", std::move(values), std::move(keys)), &result).ok());
  auto &actual = std::get<std::vector<double>>(result);
  ASSERT_EQ(actual.size(), 2U);
  EXPECT_DOUBLE_EQ(actual[0], -3.5);
  EXPECT_DOUBLE_EQ(actual[1], 0.5);
}

TEST(ListGroupTest, GroupMinSugarEmpty) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupMin", std::move(values), std::move(keys)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<int32_t>>(result).empty());
}

TEST(ListGroupTest, GroupMinSugarStringKeys) {
  auto keys = std::unique_ptr<ExecNode>(
      new ConstantListValueNode(std::vector<std::string>{"a", "b", "a", "c", "b", "a"}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 7, 5, 4, 2, 3}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupMin", std::move(values), std::move(keys)), &result).ok());
  // "a": min(1,5,3)=1, "b": min(7,2)=2, "c": 4
  std::vector<int32_t> expected = {1, 2, 4};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

// ---- GroupMin kernel (4-arg) tests -----------------------------------------

TEST(ListGroupTest, GroupMinKernelI32) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 10, 30, 20, 10}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 5, 3, 4, 2, 6}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggKernelCall("GroupMin", std::move(values), std::move(keys)), &result).ok());
  std::vector<int32_t> expected = {1, 2, 4};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expected);
}

// ---- Cross-agg correctness check -------------------------------------------

TEST(ListGroupTest, GroupMaxMinConsistency) {
  // For any non-empty group, max(group) >= min(group). Sanity check that both
  // kernels agree on which groups exist and produce the expected ordering.
  auto keys_max = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 0, 1, 1, 2}));
  auto values_max = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{3, 1, 4, 1, 5}));
  RetType max_result;
  ASSERT_TRUE(
      RunGroupAgg(MakeGroupAggSugarCall("GroupMax", std::move(values_max), std::move(keys_max)), &max_result).ok());

  auto keys_min = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 0, 1, 1, 2}));
  auto values_min = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{3, 1, 4, 1, 5}));
  RetType min_result;
  ASSERT_TRUE(
      RunGroupAgg(MakeGroupAggSugarCall("GroupMin", std::move(values_min), std::move(keys_min)), &min_result).ok());

  auto &maxes = std::get<std::vector<int32_t>>(max_result);
  auto &mins = std::get<std::vector<int32_t>>(min_result);
  ASSERT_EQ(maxes.size(), mins.size());
  for (size_t i = 0; i < maxes.size(); ++i) {
    EXPECT_GE(maxes[i], mins[i]);
  }
}

// ---- GroupMax/GroupMin error paths -----------------------------------------

TEST(ListGroupTest, GroupMaxSugarLenMismatchReturnsError) {
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3}));
  auto bad_group_index =
      std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{0U, 1U, 0U, 1U}));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(values));
  args.emplace_back(std::move(bad_group_index));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  auto expr = std::unique_ptr<ExecNode>(new FunctionNode("GroupMax", std::move(args)));

  RetType result;
  Status st = RunGroupAgg(std::move(expr), &result);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("GroupMax"), std::string::npos);
  EXPECT_NE(st.ToString().find("len mismatch"), std::string::npos);
}

TEST(ListGroupTest, GroupMinSugarLenMismatchReturnsError) {
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3}));
  auto bad_group_index =
      std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{0U, 1U, 0U, 1U}));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(values));
  args.emplace_back(std::move(bad_group_index));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  auto expr = std::unique_ptr<ExecNode>(new FunctionNode("GroupMin", std::move(args)));

  RetType result;
  Status st = RunGroupAgg(std::move(expr), &result);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("GroupMin"), std::string::npos);
  EXPECT_NE(st.ToString().find("len mismatch"), std::string::npos);
}

// ---- GroupAvg sugar form tests ---------------------------------------------

TEST(ListGroupTest, GroupAvgSugarI32) {
  // keys:   {10, 20, 10, 30, 20, 10}  -> gids: {0, 1, 0, 2, 1, 0}
  // values: {1,  2,  3,  4,  5,  6}
  //   group 0: (1+3+6)/3 = 10/3
  //   group 1: (2+5)/2   = 3.5
  //   group 2: 4/1       = 4.0
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 10, 30, 20, 10}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3, 4, 5, 6}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupAvg", std::move(values), std::move(keys)), &result).ok());
  auto &actual = std::get<std::vector<double>>(result);
  ASSERT_EQ(actual.size(), 3U);
  EXPECT_DOUBLE_EQ(actual[0], 10.0 / 3.0);
  EXPECT_DOUBLE_EQ(actual[1], 3.5);
  EXPECT_DOUBLE_EQ(actual[2], 4.0);
}

TEST(ListGroupTest, GroupAvgSugarI8NegativeValues) {
  // Integer input must promote to double (so -5/2 == -2.5, not -2). The seed
  // is 0.0, summation accumulates into double, division is floating.
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 0, 1, 1}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int8_t>{-10, 20, -3, -2}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupAvg", std::move(values), std::move(keys)), &result).ok());
  auto &actual = std::get<std::vector<double>>(result);
  ASSERT_EQ(actual.size(), 2U);
  EXPECT_DOUBLE_EQ(actual[0], 5.0);    // (-10 + 20) / 2
  EXPECT_DOUBLE_EQ(actual[1], -2.5);   // (-3 + -2) / 2   <-- int would round to -2
}

TEST(ListGroupTest, GroupAvgSugarU64) {
  // Unsigned i64 values promote to double. Accept small precision loss at
  // the ulp level, which is why we test with a modest magnitude.
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 1, 0, 1}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint64_t>{100, 200, 300, 400}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupAvg", std::move(values), std::move(keys)), &result).ok());
  auto &actual = std::get<std::vector<double>>(result);
  ASSERT_EQ(actual.size(), 2U);
  EXPECT_DOUBLE_EQ(actual[0], 200.0);  // (100 + 300) / 2
  EXPECT_DOUBLE_EQ(actual[1], 300.0);  // (200 + 400) / 2
}

TEST(ListGroupTest, GroupAvgSugarF32) {
  // f32 input still returns F64List (same promotion as ungrouped Avg).
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 0, 1, 1}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<float>{1.5F, 2.5F, 3.5F, 4.5F}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupAvg", std::move(values), std::move(keys)), &result).ok());
  auto &actual = std::get<std::vector<double>>(result);
  ASSERT_EQ(actual.size(), 2U);
  EXPECT_DOUBLE_EQ(actual[0], 2.0);
  EXPECT_DOUBLE_EQ(actual[1], 4.0);
}

TEST(ListGroupTest, GroupAvgSugarF64) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{0, 1, 0, 1, 2}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<double>{0.1, 0.2, 0.3, 0.4, 0.5}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupAvg", std::move(values), std::move(keys)), &result).ok());
  auto &actual = std::get<std::vector<double>>(result);
  ASSERT_EQ(actual.size(), 3U);
  EXPECT_DOUBLE_EQ(actual[0], 0.2);    // (0.1 + 0.3) / 2
  EXPECT_DOUBLE_EQ(actual[1], 0.3);    // (0.2 + 0.4) / 2
  EXPECT_DOUBLE_EQ(actual[2], 0.5);
}

TEST(ListGroupTest, GroupAvgSugarSingleton) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{42}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{-7}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupAvg", std::move(values), std::move(keys)), &result).ok());
  auto &actual = std::get<std::vector<double>>(result);
  ASSERT_EQ(actual.size(), 1U);
  EXPECT_DOUBLE_EQ(actual[0], -7.0);
}

TEST(ListGroupTest, GroupAvgSugarEmpty) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupAvg", std::move(values), std::move(keys)), &result).ok());
  EXPECT_TRUE(std::get<std::vector<double>>(result).empty());
}

TEST(ListGroupTest, GroupAvgSugarAllSameGroup) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{7, 7, 7, 7}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3, 4}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupAvg", std::move(values), std::move(keys)), &result).ok());
  auto &actual = std::get<std::vector<double>>(result);
  ASSERT_EQ(actual.size(), 1U);
  EXPECT_DOUBLE_EQ(actual[0], 2.5);    // (1+2+3+4)/4
}

TEST(ListGroupTest, GroupAvgSugarAllDistinctGroups) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 30, 40}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3, 4}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupAvg", std::move(values), std::move(keys)), &result).ok());
  auto &actual = std::get<std::vector<double>>(result);
  std::vector<double> expected = {1.0, 2.0, 3.0, 4.0};
  ASSERT_EQ(actual.size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_DOUBLE_EQ(actual[i], expected[i]);
  }
}

TEST(ListGroupTest, GroupAvgSugarStringKeys) {
  auto keys = std::unique_ptr<ExecNode>(
      new ConstantListValueNode(std::vector<std::string>{"a", "b", "a", "c", "b", "a"}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3, 4, 5, 6}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupAvg", std::move(values), std::move(keys)), &result).ok());
  auto &actual = std::get<std::vector<double>>(result);
  ASSERT_EQ(actual.size(), 3U);
  EXPECT_DOUBLE_EQ(actual[0], 10.0 / 3.0);  // "a": (1+3+6)/3
  EXPECT_DOUBLE_EQ(actual[1], 3.5);         // "b": (2+5)/2
  EXPECT_DOUBLE_EQ(actual[2], 4.0);         // "c": 4
}

// ---- GroupAvg kernel (4-arg) tests -----------------------------------------

TEST(ListGroupTest, GroupAvgKernelI32) {
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 10, 30, 20, 10}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3, 4, 5, 6}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggKernelCall("GroupAvg", std::move(values), std::move(keys)), &result).ok());
  auto &actual = std::get<std::vector<double>>(result);
  ASSERT_EQ(actual.size(), 3U);
  EXPECT_DOUBLE_EQ(actual[0], 10.0 / 3.0);
  EXPECT_DOUBLE_EQ(actual[1], 3.5);
  EXPECT_DOUBLE_EQ(actual[2], 4.0);
}

TEST(ListGroupTest, GroupAvgKernelSugarAgreeF64) {
  // Direct equivalence check: sugar and raw kernel must produce identical
  // results given identical inputs. Guards against future divergence (e.g.
  // sugar starting to pass a different `distinct` than GroupCount).
  auto keys_sugar = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{2, 2, 1, 2, 1}));
  auto values_sugar = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<double>{1.1, 2.2, 3.3, 4.4, 5.5}));
  RetType sugar_result;
  ASSERT_TRUE(
      RunGroupAgg(MakeGroupAggSugarCall("GroupAvg", std::move(values_sugar), std::move(keys_sugar)), &sugar_result)
          .ok());

  auto keys_kernel = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{2, 2, 1, 2, 1}));
  auto values_kernel =
      std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<double>{1.1, 2.2, 3.3, 4.4, 5.5}));
  RetType kernel_result;
  ASSERT_TRUE(
      RunGroupAgg(MakeGroupAggKernelCall("GroupAvg", std::move(values_kernel), std::move(keys_kernel)), &kernel_result)
          .ok());

  auto &sugar_vec = std::get<std::vector<double>>(sugar_result);
  auto &kernel_vec = std::get<std::vector<double>>(kernel_result);
  ASSERT_EQ(sugar_vec.size(), kernel_vec.size());
  for (size_t i = 0; i < sugar_vec.size(); ++i) {
    EXPECT_DOUBLE_EQ(sugar_vec[i], kernel_vec[i]);
  }
}

// ---- GroupAvg cross-agg identity ------------------------------------------

TEST(ListGroupTest, GroupAvgMatchesSumOverCount) {
  // For any non-empty group, Avg[g] must equal Sum[g] / count[g]. We compute
  // count[g] by Avg(1, key) which gives 1.0 per group (so count = n_g), then
  // reconstruct n_g from group_index length and distinct. Simpler: compare
  // Avg to an inline mean. This guards against any off-by-one in the counts
  // array (e.g. forgetting to memset it).
  auto keys = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 1, 2, 1, 3}));
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{10, 20, 30, 40, 50, 60}));
  RetType result;
  ASSERT_TRUE(RunGroupAgg(MakeGroupAggSugarCall("GroupAvg", std::move(values), std::move(keys)), &result).ok());
  auto &actual = std::get<std::vector<double>>(result);
  ASSERT_EQ(actual.size(), 3U);
  // group 0 (key=1): (10+30+50)/3 = 30
  // group 1 (key=2): (20+40)/2    = 30
  // group 2 (key=3): 60/1         = 60
  EXPECT_DOUBLE_EQ(actual[0], 30.0);
  EXPECT_DOUBLE_EQ(actual[1], 30.0);
  EXPECT_DOUBLE_EQ(actual[2], 60.0);
}

// ---- GroupAvg error paths --------------------------------------------------

TEST(ListGroupTest, GroupAvgKernelLenMismatchReturnsError) {
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3}));
  auto bad_group_index =
      std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{0U, 1U, 0U, 1U}));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(values));
  args.emplace_back(std::move(bad_group_index));
  args.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint32_t>(2U))));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  auto expr = std::unique_ptr<ExecNode>(new FunctionNode("GroupAvg", std::move(args)));

  RetType result;
  Status st = RunGroupAgg(std::move(expr), &result);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("GroupAvg"), std::string::npos);
  EXPECT_NE(st.ToString().find("len mismatch"), std::string::npos);
}

TEST(ListGroupTest, GroupAvgSugarLenMismatchReturnsError) {
  auto values = std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<int32_t>{1, 2, 3}));
  auto bad_group_index =
      std::unique_ptr<ExecNode>(new ConstantListValueNode(std::vector<uint32_t>{0U, 1U, 0U, 1U}));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(values));
  args.emplace_back(std::move(bad_group_index));
  args.emplace_back(std::unique_ptr<ExecNode>(new ExecContextNode()));
  auto expr = std::unique_ptr<ExecNode>(new FunctionNode("GroupAvg", std::move(args)));

  RetType result;
  Status st = RunGroupAgg(std::move(expr), &result);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("GroupAvg"), std::string::npos);
  EXPECT_NE(st.ToString().find("len mismatch"), std::string::npos);
}

}  // namespace jitfusion

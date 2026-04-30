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

}  // namespace jitfusion

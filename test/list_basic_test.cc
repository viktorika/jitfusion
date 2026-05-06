#include <cmath>
#include <cstdint>
#include <memory>
#include <numeric>
#include <variant>
#include "exec_engine.h"
#include "exec_node.h"
#include "function_registry.h"
#include "gtest/gtest.h"
#include "status.h"
#include "type.h"

using namespace jitfusion;

TEST(FunctionTest, ListConcatTest) {
  std::vector<uint32_t> l{1, 2, 3};
  std::vector<uint32_t> r{4, 5, 6};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(r));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode);
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(l_node));
  args_list.emplace_back(std::move(r_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListConcat", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect{1, 2, 3, 4, 5, 6};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, InTest) {
  uint64_t l = 10;
  std::vector<uint64_t> r = {1, 2, 10, 11};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(r));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(l_node));
  args_list.emplace_back(std::move(r_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("in", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_TRUE(static_cast<bool>(std::get<uint8_t>(result)));
}

TEST(FunctionTest, LenTest) {
  std::vector<uint64_t> l = {1, 2, 10, 11};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(l));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(l_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("Len", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint32_t>(result), l.size());
}

TEST(FunctionTest, SortAscTest1) {
  std::vector<uint8_t> data = {12, 1, 3, 2, 4};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_ctx_node = std::unique_ptr<ExecNode>(new ExecContextNode);
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_ctx_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("SortAsc", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = data;
  std::sort(expect.begin(), expect.end());
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, SortAscTest2) {
  std::vector<int64_t> data = {-1231, 3241321, -234233, 212312312, 4324};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_ctx_node = std::unique_ptr<ExecNode>(new ExecContextNode);
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_ctx_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("SortAsc", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int64_t> expect = data;
  std::sort(expect.begin(), expect.end());
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expect);
}

TEST(FunctionTest, SortAscTest3) {
  std::vector<double> data = {-1231.12321, 3241321.111, -234233.444, 212312312.12312, 4324.12312};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_ctx_node = std::unique_ptr<ExecNode>(new ExecContextNode);
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_ctx_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("SortAsc", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = data;
  std::sort(expect.begin(), expect.end());
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, TruncateTest1) {
  std::vector<double> data = {-1231.12321, 3241321.111, -234233.444, 212312312.12312, 4324.12312};
  uint32_t size = 2;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto size_node = std::unique_ptr<ExecNode>(new ConstantValueNode(size));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(size_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("Truncate", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect(data.begin(), data.begin() + size);
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, TruncateTest2) {
  std::vector<uint8_t> data = {12, 1, 3, 2, 4};
  uint32_t size = 3;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto size_node = std::unique_ptr<ExecNode>(new ConstantValueNode(size));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(size_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("Truncate", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect(data.begin(), data.begin() + size);
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, TruncateTest3) {
  std::vector<std::string> data = {"abc", "edg", "sda", "a", "c"};
  uint32_t size = 4;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto size_node = std::unique_ptr<ExecNode>(new ConstantValueNode(size));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(size_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("Truncate", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<std::string> expect(data.begin(), data.begin() + size);
  EXPECT_EQ(std::get<std::vector<std::string>>(result), expect);
}

TEST(FunctionTest, TruncateTest4) {
  std::vector<std::string> data = {"abc", "edg", "sda", "a", "c"};
  uint32_t size = 10;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto size_node = std::unique_ptr<ExecNode>(new ConstantValueNode(size));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(size_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("Truncate", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<std::string> expect(data.begin(), data.end());
  EXPECT_EQ(std::get<std::vector<std::string>>(result), expect);
}

namespace {
std::vector<std::string> RunCrossJoin(const std::vector<std::string>& a, const std::vector<std::string>& b,
                                      const std::string& sep) {
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto a_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(a));
  auto b_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(b));
  auto sep_node = std::unique_ptr<ExecNode>(new ConstantValueNode(sep));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode);
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(a_node));
  args_list.emplace_back(std::move(b_node));
  args_list.emplace_back(std::move(sep_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("CrossJoin", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  EXPECT_TRUE(st.ok()) << st.ToString();
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  return std::get<std::vector<std::string>>(result);
}
}  // namespace

TEST(FunctionTest, CrossJoinBasicTest) {
  // Mirrors the spec example: outer-major (a is outer, b is inner).
  auto got = RunCrossJoin({"1", "2", "3"}, {"a", "b", "c"}, "_");
  std::vector<std::string> expect = {"1_a", "1_b", "1_c", "2_a", "2_b", "2_c", "3_a", "3_b", "3_c"};
  EXPECT_EQ(got, expect);
}

TEST(FunctionTest, CrossJoinEmptySepTest) {
  auto got = RunCrossJoin({"x", "y"}, {"1", "2"}, "");
  std::vector<std::string> expect = {"x1", "x2", "y1", "y2"};
  EXPECT_EQ(got, expect);
}

TEST(FunctionTest, CrossJoinMultiCharSepTest) {
  auto got = RunCrossJoin({"foo", "bar"}, {"baz"}, "::");
  std::vector<std::string> expect = {"foo::baz", "bar::baz"};
  EXPECT_EQ(got, expect);
}

TEST(FunctionTest, CrossJoinSingletonTest) {
  // 1xN and Nx1 shapes.
  EXPECT_EQ(RunCrossJoin({"a"}, {"x", "y", "z"}, "-"), (std::vector<std::string>{"a-x", "a-y", "a-z"}));
  EXPECT_EQ(RunCrossJoin({"x", "y", "z"}, {"a"}, "-"), (std::vector<std::string>{"x-a", "y-a", "z-a"}));
}

TEST(FunctionTest, CrossJoinEmptyInputTest) {
  // Either side empty -> empty result.
  EXPECT_TRUE(RunCrossJoin({}, {"a", "b"}, "_").empty());
  EXPECT_TRUE(RunCrossJoin({"a", "b"}, {}, "_").empty());
  EXPECT_TRUE(RunCrossJoin({}, {}, "_").empty());
}

TEST(FunctionTest, CrossJoinEmptyElementTest) {
  // Empty strings in inputs are valid and should produce sep-only joins.
  auto got = RunCrossJoin({"", "a"}, {"", "b"}, "_");
  std::vector<std::string> expect = {"_", "_b", "a_", "a_b"};
  EXPECT_EQ(got, expect);
}

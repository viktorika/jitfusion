/*
 * @Author: victorika
 * @Date: 2025-01-24 10:40:25
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-29 22:46:21
 */
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

TEST(FunctionTest, ExpTest) {
  uint32_t arg = 8;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("exp", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<double>(result), std::exp(arg));
}

TEST(FunctionTest, LogTest) {
  float arg = 233213.12312;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("log", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<float>(result), std::log(arg));
}

TEST(FunctionTest, Log2Test) {
  int64_t arg = 3432432;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("log2", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<double>(result), std::log2(arg));
}

TEST(FunctionTest, Log10Test) {
  double arg = 3432432.12312;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("log10", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<double>(result), std::log10(arg));
}

TEST(FunctionTest, FracTest) {
  double arg = 3432432.12312;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("frac", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<double>(result), arg - std::trunc(arg));
}

TEST(FunctionTest, TruncTest) {
  float arg = 3432432.12312;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("trunc", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<float>(result), std::trunc(arg));
}

TEST(FunctionTest, PowTest) {
  int32_t arg = 3432432;
  int32_t x = 3;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  auto x_node = std::unique_ptr<ExecNode>(new ConstantValueNode(x));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(x_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("pow", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<double>(result), std::pow(arg, x));
}

TEST(FunctionTest, SinTest) {
  double arg = 3432432;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("sin", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<double>(result), std::sin(arg));
}

TEST(FunctionTest, CosTest) {
  float arg = 3432432;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("cos", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<float>(result), std::cos(arg));
}

TEST(FunctionTest, tanTest) {
  float arg = 3432432;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("tan", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<float>(result), std::tan(arg));
}

TEST(FunctionTest, SqrtTest) {
  double arg = 3432432;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("sqrt", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<double>(result), std::sqrt(arg));
}

TEST(FunctionTest, AbsTest) {
  int8_t arg = -4;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("abs", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), std::abs(arg));
}

TEST(FunctionTest, CeilTest) {
  float arg = -4;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ceil", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<float>(result), std::ceil(arg));
}

TEST(FunctionTest, FloorTest) {
  float arg = -4;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("floor", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<float>(result), std::floor(arg));
}

TEST(FunctionTest, RoundTest) {
  float arg = -4;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("round", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<float>(result), std::round(arg));
}

TEST(FunctionTest, MinTest) {
  float l = -4;
  float r = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(l_node));
  args_list.emplace_back(std::move(r_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("min", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<float>(result), std::min(l, r));
}

TEST(FunctionTest, MaxTest) {
  double l = -4;
  double r = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(l_node));
  args_list.emplace_back(std::move(r_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("max", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<double>(result), std::max(l, r));
}

TEST(FunctionTest, StringConcatTest) {
  std::string l = "abc";
  std::string r = "def";
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto ctx_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(l_node));
  args_list.emplace_back(std::move(r_node));
  args_list.emplace_back(std::move(ctx_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("StringConcat", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::string>(result), l + r);
}

TEST(FunctionTest, StringCmpTest) {
  std::string l = "abc";
  std::string r = "def";
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(l_node));
  args_list.emplace_back(std::move(r_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("StringCmp", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), strcmp(l.data(), r.data()));
}

TEST(FunctionTest, StringLenTest) {
  std::string l = "abc";
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(l_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("StringLen", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint32_t>(result), l.length());
}

TEST(FunctionTest, ListConcatTest) {
  std::vector<uint32_t> l{1, 2, 3};
  std::vector<uint32_t> r{4, 5, 6};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(r));
  auto ctx_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(l_node));
  args_list.emplace_back(std::move(r_node));
  args_list.emplace_back(std::move(ctx_node));
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

TEST(FunctionTest, CastTest) {
  uint32_t arg = 8;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(arg));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("CastU8", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint8_t>(result), arg);
}

TEST(FunctionTest, SumTest1) {
  std::vector<uint8_t> data = {1, 2, 10, 11};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("Sum", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint64_t>(result), std::accumulate(data.begin(), data.end(), 0));
}

TEST(FunctionTest, SumTest2) {
  std::vector<float> data = {1.2, 2.3, 10.12321, 11.32423432};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("Sum", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<double>(result), std::accumulate(data.begin(), data.end(), 0.0));
}
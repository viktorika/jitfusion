/*
 * @Author: victorika
 * @Date: 2025-01-24 10:40:25
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-30 22:23:43
 */
#include <cmath>
#include <cstdint>
#include <memory>
#include <variant>
#include "exec_engine.h"
#include "exec_node.h"
#include "function_registry.h"
#include "gtest/gtest.h"
#include "status.h"
#include "type.h"

using namespace jitfusion;

TEST(IfTest, TrueTest) {
  auto condition_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(1)));
  auto true_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(1000)));
  auto false_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(-1000)));
  std::vector<std::unique_ptr<ExecNode>> child;
  child.emplace_back(std::move(condition_node));
  child.emplace_back(std::move(true_node));
  child.emplace_back(std::move(false_node));
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new IfNode(std::move(child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 1000);
}

TEST(IfTest, FalseTest) {
  auto condition_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(0)));
  auto true_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(1000)));
  auto false_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(-1000)));
  std::vector<std::unique_ptr<ExecNode>> child;
  child.emplace_back(std::move(condition_node));
  child.emplace_back(std::move(true_node));
  child.emplace_back(std::move(false_node));
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new IfNode(std::move(child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), -1000);
}

TEST(IfTest, Test1) {
  auto condition_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(0)));
  auto true_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint64_t>(100)));
  auto false_node = std::unique_ptr<ExecNode>(new ConstantValueNode(0.1024));
  std::vector<std::unique_ptr<ExecNode>> child;
  child.emplace_back(std::move(condition_node));
  child.emplace_back(std::move(true_node));
  child.emplace_back(std::move(false_node));
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new IfNode(std::move(child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<double>(result), 0.1024);
}

TEST(IfTest, Test2) {
  auto condition_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint16_t>(256)));
  auto true_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint64_t>(100)));
  auto false_node = std::unique_ptr<ExecNode>(new ConstantValueNode(0.1024));
  std::vector<std::unique_ptr<ExecNode>> child;
  child.emplace_back(std::move(condition_node));
  child.emplace_back(std::move(true_node));
  child.emplace_back(std::move(false_node));
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new IfNode(std::move(child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<double>(result), 100);
}

TEST(IfTest, Test3) {
  auto condition_node = std::unique_ptr<ExecNode>(new ConstantValueNode(3.14));
  auto true_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint64_t>(100)));
  auto false_node = std::unique_ptr<ExecNode>(new ConstantValueNode(0.1024));
  std::vector<std::unique_ptr<ExecNode>> child;
  child.emplace_back(std::move(condition_node));
  child.emplace_back(std::move(true_node));
  child.emplace_back(std::move(false_node));
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new IfNode(std::move(child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<double>(result), 100);
}

TEST(IfTest, Test4) {
  auto condition_node = std::unique_ptr<ExecNode>(new ConstantValueNode(0.0));
  auto true_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint64_t>(100)));
  auto false_node = std::unique_ptr<ExecNode>(new ConstantValueNode(0.1024));
  std::vector<std::unique_ptr<ExecNode>> child;
  child.emplace_back(std::move(condition_node));
  child.emplace_back(std::move(true_node));
  child.emplace_back(std::move(false_node));
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new IfNode(std::move(child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<double>(result), 0.1024);
}

TEST(IfTest, NestedIfInThenBranch) {
  auto inner_cond = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(1)));
  auto inner_true = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(100)));
  auto inner_false = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(200)));
  std::vector<std::unique_ptr<ExecNode>> inner_child;
  inner_child.emplace_back(std::move(inner_cond));
  inner_child.emplace_back(std::move(inner_true));
  inner_child.emplace_back(std::move(inner_false));
  auto inner_if = std::unique_ptr<ExecNode>(new IfNode(std::move(inner_child)));

  auto outer_cond = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(1)));
  auto outer_false = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(300)));
  std::vector<std::unique_ptr<ExecNode>> outer_child;
  outer_child.emplace_back(std::move(outer_cond));
  outer_child.emplace_back(std::move(inner_if));
  outer_child.emplace_back(std::move(outer_false));

  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new IfNode(std::move(outer_child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 100);
}

TEST(IfTest, NestedIfInElseBranch) {
  auto inner_cond = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(0)));
  auto inner_true = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(400)));
  auto inner_false = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(500)));
  std::vector<std::unique_ptr<ExecNode>> inner_child;
  inner_child.emplace_back(std::move(inner_cond));
  inner_child.emplace_back(std::move(inner_true));
  inner_child.emplace_back(std::move(inner_false));
  auto inner_if = std::unique_ptr<ExecNode>(new IfNode(std::move(inner_child)));

  auto outer_cond = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(0)));
  auto outer_true = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(300)));
  std::vector<std::unique_ptr<ExecNode>> outer_child;
  outer_child.emplace_back(std::move(outer_cond));
  outer_child.emplace_back(std::move(outer_true));
  outer_child.emplace_back(std::move(inner_if));

  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new IfNode(std::move(outer_child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 500);
}

TEST(IfTest, DoubleNestedIf) {
  auto innermost_cond = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(1)));
  auto innermost_true = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(20)));
  auto innermost_false = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(30)));
  std::vector<std::unique_ptr<ExecNode>> innermost_child;
  innermost_child.emplace_back(std::move(innermost_cond));
  innermost_child.emplace_back(std::move(innermost_true));
  innermost_child.emplace_back(std::move(innermost_false));
  auto innermost_if = std::unique_ptr<ExecNode>(new IfNode(std::move(innermost_child)));

  auto mid_cond = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(0)));
  auto mid_true = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(10)));
  std::vector<std::unique_ptr<ExecNode>> mid_child;
  mid_child.emplace_back(std::move(mid_cond));
  mid_child.emplace_back(std::move(mid_true));
  mid_child.emplace_back(std::move(innermost_if));
  auto mid_if = std::unique_ptr<ExecNode>(new IfNode(std::move(mid_child)));

  auto outer_cond = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(1)));
  auto outer_false = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(40)));
  std::vector<std::unique_ptr<ExecNode>> outer_child;
  outer_child.emplace_back(std::move(outer_cond));
  outer_child.emplace_back(std::move(mid_if));
  outer_child.emplace_back(std::move(outer_false));

  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new IfNode(std::move(outer_child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 20);
}

TEST(IfTest, IfWithNestedSwitch) {
  std::vector<std::unique_ptr<ExecNode>> switch_child;
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(0))));
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(10))));
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(1))));
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(20))));
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(30))));
  auto inner_switch = std::unique_ptr<ExecNode>(new SwitchNode(std::move(switch_child)));

  auto outer_cond = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(1)));
  auto outer_false = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(40)));
  std::vector<std::unique_ptr<ExecNode>> outer_child;
  outer_child.emplace_back(std::move(outer_cond));
  outer_child.emplace_back(std::move(inner_switch));
  outer_child.emplace_back(std::move(outer_false));

  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new IfNode(std::move(outer_child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 20);
}
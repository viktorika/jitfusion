/*
 * @Author: victorika
 * @Date: 2025-01-24 10:40:25
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-30 22:37:59
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

TEST(SwitchTest, Test1) {
  std::vector<uint8_t> condition_list = {0, 1, 0, 1};
  std::vector<int32_t> value_list = {100, 200, 300, 400};
  std::vector<std::unique_ptr<ExecNode>> child;
  for (size_t i = 0; i < condition_list.size(); i++) {
    child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(condition_list[i])));
    child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(value_list[i])));
  }
  child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(500)));
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new SwitchNode(std::move(child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 200);
}

TEST(SwitchTest, Test2) {
  std::vector<uint8_t> condition_list = {0, 0, 0, 1};
  std::vector<int32_t> value_list = {100, 200, 300, 400};
  std::vector<std::unique_ptr<ExecNode>> child;
  for (size_t i = 0; i < condition_list.size(); i++) {
    child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(condition_list[i])));
    child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(value_list[i])));
  }
  child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(500)));
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new SwitchNode(std::move(child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 400);
}

TEST(SwitchTest, Test3) {
  std::vector<uint8_t> condition_list = {0, 0, 0, 0};
  std::vector<int32_t> value_list = {100, 200, 300, 400};
  std::vector<std::unique_ptr<ExecNode>> child;
  for (size_t i = 0; i < condition_list.size(); i++) {
    child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(condition_list[i])));
    child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(value_list[i])));
  }
  child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(500)));
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new SwitchNode(std::move(child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 500);
}

TEST(SwitchTest, Test4) {
  std::vector<uint32_t> condition_list = {256, 256, 0, 0};
  std::vector<int32_t> value_list = {100, 200, 300, 400};
  std::vector<std::unique_ptr<ExecNode>> child;
  for (size_t i = 0; i < condition_list.size(); i++) {
    child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(condition_list[i])));
    child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(value_list[i])));
  }
  child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(500)));
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new SwitchNode(std::move(child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 100);
}


TEST(SwitchTest, Test5) {
  std::vector<float> condition_list = {0, 3.14, 0, 0};
  std::vector<int32_t> value_list = {100, 200, 300, 400};
  std::vector<std::unique_ptr<ExecNode>> child;
  for (size_t i = 0; i < condition_list.size(); i++) {
    child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(condition_list[i])));
    child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(value_list[i])));
  }
  child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(500)));
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new SwitchNode(std::move(child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 200);
}

TEST(SwitchTest, SwitchWithNestedIf) {
  auto inner_cond = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(1)));
  auto inner_true = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(20)));
  auto inner_false = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(30)));
  std::vector<std::unique_ptr<ExecNode>> inner_child;
  inner_child.emplace_back(std::move(inner_cond));
  inner_child.emplace_back(std::move(inner_true));
  inner_child.emplace_back(std::move(inner_false));
  auto inner_if = std::unique_ptr<ExecNode>(new IfNode(std::move(inner_child)));

  std::vector<std::unique_ptr<ExecNode>> switch_child;
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(0))));
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(10))));
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(1))));
  switch_child.emplace_back(std::move(inner_if));
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(40))));

  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new SwitchNode(std::move(switch_child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 20);
}

TEST(SwitchTest, SwitchDefaultWithNestedIf) {
  auto inner_cond = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(1)));
  auto inner_true = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(99)));
  auto inner_false = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(88)));
  std::vector<std::unique_ptr<ExecNode>> inner_child;
  inner_child.emplace_back(std::move(inner_cond));
  inner_child.emplace_back(std::move(inner_true));
  inner_child.emplace_back(std::move(inner_false));
  auto inner_if = std::unique_ptr<ExecNode>(new IfNode(std::move(inner_child)));

  std::vector<std::unique_ptr<ExecNode>> switch_child;
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(0))));
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(10))));
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(0))));
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(20))));
  switch_child.emplace_back(std::move(inner_if));

  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new SwitchNode(std::move(switch_child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 99);
}

TEST(SwitchTest, NestedSwitch) {
  std::vector<std::unique_ptr<ExecNode>> inner_switch_child;
  inner_switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(0))));
  inner_switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(100))));
  inner_switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(1))));
  inner_switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(200))));
  inner_switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(300))));
  auto inner_switch = std::unique_ptr<ExecNode>(new SwitchNode(std::move(inner_switch_child)));

  std::vector<std::unique_ptr<ExecNode>> outer_switch_child;
  outer_switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(0))));
  outer_switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(10))));
  outer_switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(1))));
  outer_switch_child.emplace_back(std::move(inner_switch));
  outer_switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(40))));

  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new SwitchNode(std::move(outer_switch_child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 200);
}

TEST(SwitchTest, SwitchWithDoubleNestedIf) {
  auto innermost_cond = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(1)));
  auto innermost_true = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(77)));
  auto innermost_false = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(88)));
  std::vector<std::unique_ptr<ExecNode>> innermost_child;
  innermost_child.emplace_back(std::move(innermost_cond));
  innermost_child.emplace_back(std::move(innermost_true));
  innermost_child.emplace_back(std::move(innermost_false));
  auto innermost_if = std::unique_ptr<ExecNode>(new IfNode(std::move(innermost_child)));

  auto mid_cond = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(0)));
  auto mid_false = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(99)));
  std::vector<std::unique_ptr<ExecNode>> mid_child;
  mid_child.emplace_back(std::move(mid_cond));
  mid_child.emplace_back(std::move(innermost_if));
  mid_child.emplace_back(std::move(mid_false));
  auto mid_if = std::unique_ptr<ExecNode>(new IfNode(std::move(mid_child)));

  std::vector<std::unique_ptr<ExecNode>> switch_child;
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(0))));
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(10))));
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint8_t>(1))));
  switch_child.emplace_back(std::move(mid_if));
  switch_child.emplace_back(std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<int32_t>(40))));

  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto node = std::unique_ptr<ExecNode>(new SwitchNode(std::move(switch_child)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 99);
}
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
  for (int i = 0; i < condition_list.size(); i++) {
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
  for (int i = 0; i < condition_list.size(); i++) {
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
  for (int i = 0; i < condition_list.size(); i++) {
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
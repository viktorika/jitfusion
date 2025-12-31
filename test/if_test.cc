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
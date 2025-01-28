/*
 * @Author: victorika
 * @Date: 2025-01-24 10:40:25
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-28 22:20:52
 */
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

TEST(UnaryOPTest, PlusTest) {
  int64_t value = -1000000;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto op_node = std::unique_ptr<ExecNode>(new UnaryOPNode(UnaryOPType::kPlus, std::move(args_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int64_t>(result), value);
}

TEST(UnaryOPTest, MinusTest) {
  int32_t value = -1000000;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto op_node = std::unique_ptr<ExecNode>(new UnaryOPNode(UnaryOPType::kMinus, std::move(args_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), -value);
}

TEST(UnaryOPTest, NotTest) {
  uint16_t value = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto op_node = std::unique_ptr<ExecNode>(new UnaryOPNode(UnaryOPType::kNot, std::move(args_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint8_t>(result), !value);
}

TEST(UnaryOPTest, BitwiseNotTest) {
  int8_t value = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto op_node = std::unique_ptr<ExecNode>(new UnaryOPNode(UnaryOPType::kBitwiseNot, std::move(args_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int8_t>(result), ~value);
}
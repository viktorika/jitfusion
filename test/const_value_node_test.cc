/*
 * @Author: victorika
 * @Date: 2025-01-24 10:40:25
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-28 21:10:40
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

TEST(ConstValueTest, U8Test) {
  const uint8_t value = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint8_t>(result), value);
}

TEST(ConstValueTest, I8Test) {
  const int8_t value = -100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int8_t>(result), value);
}

TEST(ConstValueTest, U16Test) {
  const uint16_t value = 200;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint16_t>(result), value);
}

TEST(ConstValueTest, I16Test) {
  const int16_t value = -200;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int16_t>(result), value);
}

TEST(ConstValueTest, U32Test) {
  const uint32_t value = 3423423423;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint32_t>(result), value);
}

TEST(ConstValueTest, I32Test) {
  const int32_t value = -423423423;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), value);
}

TEST(ConstValueTest, U64Test) {
  const uint64_t value = 23412343124399;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint64_t>(result), value);
}

TEST(ConstValueTest, I64Test) {
  const int64_t value = -23412343124399;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int64_t>(result), value);
}

TEST(ConstValueTest, F32Test) {
  const float value = 1.23;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<float>(result), value);
}

TEST(ConstValueTest, F64Test) {
  const double value = 2312312.12312312;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<double>(result), value);
}
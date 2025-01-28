/*
 * @Author: victorika
 * @Date: 2025-01-24 10:40:25
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-28 21:20:20
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

TEST(ConstListValueTest, U8ListTest) {
  const std::vector<uint8_t> value = {100, 200, 1, 2};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), value);
}

TEST(ConstListValueTest, I8ListTest) {
  const std::vector<int8_t> value = {100, -10, 1, 2};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::vector<int8_t>>(result), value);
}

TEST(ConstListValueTest, U16ListTest) {
  const std::vector<uint16_t> value = {100, 200, 1, 2};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::vector<uint16_t>>(result), value);
}

TEST(ConstListValueTest, I16ListTest) {
  const std::vector<int16_t> value = {100, -10, 1, 2};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::vector<int16_t>>(result), value);
}

TEST(ConstListValueTest, U32ListTest) {
  const std::vector<uint32_t> value = {100, 200, 1, 2};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), value);
}

TEST(ConstListValueTest, I32ListTest) {
  const std::vector<int32_t> value = {100, -10, 1, 2};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), value);
}

TEST(ConstListValueTest, U64ListTest) {
  const std::vector<uint64_t> value = {100, 200, 1, 2};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::vector<uint64_t>>(result), value);
}

TEST(ConstListValueTest, I64ListTest) {
  const std::vector<int64_t> value = {100, -10, 1, 2};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), value);
}

TEST(ConstListValueTest, F32ListTest) {
  const std::vector<float> value = {1.23, 12312.12312, 345345.435345, 235443.5654};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::vector<float>>(result), value);
}

TEST(ConstListValueTest, F64ListTest) {
  const std::vector<double> value = {1.23, 12312.12312, 345345.435345, 235443.5654};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::vector<double>>(result), value);
}

TEST(ConstListValueTest, StringListTest) {
  const std::vector<std::string> value = {"aaa", "bbb", "ccc", "ddd"};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(args_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::vector<std::string>>(result), value);
}
/*
 * @Author: victorika
 * @Date: 2025-01-24 10:40:25
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-28 22:40:14
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

TEST(BinaryOPTest, AddTest) {
  uint8_t l = 100;
  uint16_t r = 1231;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node = std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kAdd, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint16_t>(result), l + r);
}

TEST(BinaryOPTest, SubTest) {
  uint8_t l = 100;
  uint32_t r = 1231;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node = std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kSub, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint32_t>(result), l - r);
}

TEST(BinaryOPTest, MulTest) {
  uint8_t l = 100;
  uint64_t r = 1231;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node = std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kMul, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint64_t>(result), l * r);
}

TEST(BinaryOPTest, DivTest) {
  uint16_t l = 2000;
  uint32_t r = 12;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node = std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kDiv, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint32_t>(result), l / r);
}

TEST(BinaryOPTest, ModTest) {
  uint16_t l = 2000;
  uint64_t r = 12;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node = std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kMod, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint64_t>(result), l % r);
}

TEST(BinaryOPTest, LargeTest) {
  uint32_t l = 2000;
  uint64_t r = 12;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node =
      std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kLarge, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(static_cast<bool>(std::get<uint8_t>(result)), l > r);
}

TEST(BinaryOPTest, LargeEqualTest) {
  uint8_t l = 2;
  uint8_t r = 12;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node =
      std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kLargeEqual, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(static_cast<bool>(std::get<uint8_t>(result)), l >= r);
}

TEST(BinaryOPTest, EqualTest) {
  uint16_t l = 2;
  uint16_t r = 12;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node =
      std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kEqual, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(static_cast<bool>(std::get<uint8_t>(result)), l == r);
}

TEST(BinaryOPTest, LessTest) {
  uint32_t l = 2;
  uint32_t r = 12;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node = std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kLess, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(static_cast<bool>(std::get<uint8_t>(result)), l < r);
}

TEST(BinaryOPTest, LessEqualTest) {
  uint32_t l = 2;
  uint32_t r = 12;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node =
      std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kLessEqual, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(static_cast<bool>(std::get<uint8_t>(result)), l <= r);
}

TEST(BinaryOPTest, NotEqualTest) {
  uint64_t l = 2;
  uint64_t r = 12;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node =
      std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kNotEqual, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(static_cast<bool>(std::get<uint8_t>(result)), l != r);
}

TEST(BinaryOPTest, AndTest) {
  uint16_t l = 20000;
  uint8_t r = 119;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node = std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kAnd, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(static_cast<bool>(std::get<uint8_t>(result)), l && r);
}

TEST(BinaryOPTest, OrTest) {
  uint32_t l = 32414132;
  uint16_t r = 64123;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node = std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kOr, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(static_cast<bool>(std::get<uint8_t>(result)), l || r);
}

TEST(BinaryOPTest, BitwiseAndTest) {
  uint64_t l = 32414132;
  uint32_t r = 64123;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node =
      std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kBitwiseAnd, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint64_t>(result), l & r);
}

TEST(BinaryOPTest, BitwiseOrTest) {
  uint64_t l = 32414132;
  uint32_t r = 64123;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node =
      std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kBitwiseOr, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint64_t>(result), l | r);
}

TEST(BinaryOPTest, BitwiseXorTest) {
  uint64_t l = 32414132;
  uint32_t r = 64123;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node =
      std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kBitwiseXor, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint64_t>(result), l ^ r);
}

TEST(BinaryOPTest, BitwiseShiftLeftTest) {
  uint64_t l = 32414132;
  uint32_t r = 3;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node = std::unique_ptr<ExecNode>(
      new BinaryOPNode(BinaryOPType::kBitwiseShiftLeft, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint64_t>(result), l << r);
}

TEST(BinaryOPTest, BitwiseShiftRightTest) {
  uint64_t l = 32414132;
  uint32_t r = 3;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node = std::unique_ptr<ExecNode>(
      new BinaryOPNode(BinaryOPType::kBitwiseShiftRight, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint64_t>(result), l >> r);
}

TEST(BinaryOPTest, StringConcatTest) {
  std::string l = "123";
  std::string r = "456";
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(l));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(r));
  auto op_node = std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kAdd, std::move(l_node), std::move(r_node)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<std::string>(result), l + r);
}
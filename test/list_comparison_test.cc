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

TEST(FunctionTest, GenLargeBitmapTest1) {
  std::vector<uint16_t> data = {1, 2, 3, 4, 5, 6, 7};
  uint16_t value = 3;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenLargeBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {120};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenLargeBitmapTest2) {
  std::vector<int64_t> data = {-1, 2, 3, -4, 5, -6, 7, -8, -9, 10};
  int64_t value = 0;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenLargeBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {86, 2};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenLargeBitmapTest3) {
  std::vector<double> data = {-1, 2, 3, -4, 5, -6, 7, 8};
  double value = 0;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenLargeBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {214};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenLargeBitmapTest4) {
  std::vector<uint16_t> data = {1, 2, 3, 4};
  std::vector<uint16_t> value = {1, 5, 2, 7};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenLargeBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {4};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenLargeBitmapTest5) {
  std::vector<int64_t> data = {-1, 2, 3, -4, 5, -6, 7, -8, -9, 10};
  std::vector<int64_t> value = {-1, -2, -3, -4, -5, -6, -7, -8, -9, -10};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenLargeBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {86, 2};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenLargeBitmapTest6) {
  std::vector<double> data = {-1, 2, 3, -4, 5, -6, 7, 8};
  std::vector<double> value = {0, 0, 0, 0, 0, 0, 0, 0};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenLargeBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {214};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenLargeEqualBitmapTest1) {
  std::vector<uint16_t> data = {1, 2, 3, 4, 5, 6, 7};
  uint16_t value = 3;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenLargeEqualBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {124};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenLargeEqualBitmapTest2) {
  std::vector<uint16_t> data = {1, 2, 3, 4, 5, 6, 7};
  std::vector<uint16_t> value = {2, 1, 4, 3, 5, 5, 5};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenLargeEqualBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {122};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenEqualBitmapTest1) {
  std::vector<int64_t> data = {-1, 2, 3, -4, 5, -6, 7, -8, -9, 10};
  int64_t value = 5;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenEqualBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {16, 0};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenEqualBitmapTest2) {
  std::vector<int64_t> data = {-1, 2, 3, -4, 5, -6, 7, -8, -9, 10};
  std::vector<int64_t> value = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenEqualBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {86, 2};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenLessBitmapTest1) {
  std::vector<double> data = {-1, 2, 3, -4, 5, -6, 7, 8};
  double value = 0;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenLessBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {41};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenLessBitmapTest2) {
  std::vector<double> data = {-1, 2, 3, -4, 5, -6, 7, 8};
  std::vector<double> value = {0, 0, 0, 0, 0, 0, 0, 0};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenLessBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {41};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenLessEqualBitmapTest1) {
  std::vector<uint8_t> data = {1, 2, 3, 4, 5, 6, 7, 8};
  uint8_t value = 1;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenLessEqualBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {1};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenLessEqualBitmapTest2) {
  std::vector<uint8_t> data = {1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<uint8_t> value = {1, 1, 1, 1, 1, 1, 1, 1};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenLessEqualBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {1};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenNotEqualBitmapTest1) {
  std::vector<uint32_t> data = {1, 2, 3, 4, 5, 6, 7, 8};
  uint32_t value = 10;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenNotEqualBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {255};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, GenNotEqualBitmapTest2) {
  std::vector<uint32_t> data = {1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<uint32_t> value = {10, 10, 10, 10, 10, 10, 10, 10};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GenNotEqualBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {255};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

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

TEST(FunctionTest, IfLargeTest1) {
  std::vector<int8_t> data = {1, 2, 3, 4, 5};
  int8_t cmp_value = 3;
  int8_t fill_value = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto cmp_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(cmp_value));
  auto fill_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(fill_value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(cmp_value_node));
  args_list.emplace_back(std::move(fill_value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("IfLarge", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int8_t> expect = {1, 2, 3, 100, 100};
  EXPECT_EQ(std::get<std::vector<int8_t>>(result), expect);
}

TEST(FunctionTest, IfLargeTest2) {
  std::vector<uint32_t> data = {1, 2, 3, 4, 5};
  uint32_t cmp_value = 3;
  uint32_t fill_value = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto cmp_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(cmp_value));
  auto fill_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(fill_value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(cmp_value_node));
  args_list.emplace_back(std::move(fill_value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("IfLarge", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {1, 2, 3, 100, 100};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, IfLargeTest3) {
  std::vector<float> data = {1, 2, 3, 4, 5};
  float cmp_value = 3;
  float fill_value = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto cmp_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(cmp_value));
  auto fill_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(fill_value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(cmp_value_node));
  args_list.emplace_back(std::move(fill_value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("IfLarge", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<float> expect = {1, 2, 3, 100, 100};
  EXPECT_EQ(std::get<std::vector<float>>(result), expect);
}

TEST(FunctionTest, IfLargeEqualTest) {
  std::vector<int8_t> data = {1, 2, 3, 4, 5};
  int8_t cmp_value = 3;
  int8_t fill_value = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto cmp_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(cmp_value));
  auto fill_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(fill_value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(cmp_value_node));
  args_list.emplace_back(std::move(fill_value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("IfLargeEqual", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int8_t> expect = {1, 2, 100, 100, 100};
  EXPECT_EQ(std::get<std::vector<int8_t>>(result), expect);
}

TEST(FunctionTest, IfEqualTest) {
  std::vector<int8_t> data = {1, 2, 3, 4, 5};
  int8_t cmp_value = 3;
  int8_t fill_value = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto cmp_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(cmp_value));
  auto fill_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(fill_value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(cmp_value_node));
  args_list.emplace_back(std::move(fill_value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("IfEqual", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int8_t> expect = {1, 2, 100, 4, 5};
  EXPECT_EQ(std::get<std::vector<int8_t>>(result), expect);
}

TEST(FunctionTest, IfELessTest) {
  std::vector<uint64_t> data = {1, 2, 3, 4, 5};
  uint64_t cmp_value = 3;
  uint64_t fill_value = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto cmp_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(cmp_value));
  auto fill_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(fill_value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(cmp_value_node));
  args_list.emplace_back(std::move(fill_value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("IfLess", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint64_t> expect = {100, 100, 3, 4, 5};
  EXPECT_EQ(std::get<std::vector<uint64_t>>(result), expect);
}

TEST(FunctionTest, IfELessEqualTest) {
  std::vector<double> data = {1, 2, 3, 4, 5};
  double cmp_value = 3;
  double fill_value = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto cmp_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(cmp_value));
  auto fill_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(fill_value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(cmp_value_node));
  args_list.emplace_back(std::move(fill_value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("IfLessEqual", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {100, 100, 100, 4, 5};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, IfNotEqualTest) {
  std::vector<double> data = {1, 2, 3, 4, 5};
  double cmp_value = 3;
  double fill_value = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto cmp_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(cmp_value));
  auto fill_value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(fill_value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(cmp_value_node));
  args_list.emplace_back(std::move(fill_value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("IfNotEqual", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {100, 100, 3, 100, 100};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, IfByBitmapTest1) {
  std::vector<uint8_t> bitmap = {0xFF, 0x1, 0x2, 0x5};
  std::vector<int32_t> lhs = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                              17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
  int32_t rhs = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto bitmap_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(bitmap));
  auto lhs_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(lhs));
  auto rhs_node = std::unique_ptr<ExecNode>(new ConstantValueNode(rhs));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(bitmap_node));
  args_list.emplace_back(std::move(lhs_node));
  args_list.emplace_back(std::move(rhs_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("IfByBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int32_t> expect = {1,   2,  3,   4,   5,   6,   7,   8,   9,  100, 100, 100, 100, 100, 100, 100,
                                 100, 18, 100, 100, 100, 100, 100, 100, 25, 100, 27,  100, 100, 100, 100, 100};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expect);
}

TEST(FunctionTest, IfByBitmapTest2) {
  std::vector<uint8_t> bitmap = {0xFF, 0x1, 0x2, 0x5};
  double lhs = 100;
  std::vector<double> rhs = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                             17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};

  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto bitmap_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(bitmap));
  auto lhs_node = std::unique_ptr<ExecNode>(new ConstantValueNode(lhs));
  auto rhs_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(rhs));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(bitmap_node));
  args_list.emplace_back(std::move(lhs_node));
  args_list.emplace_back(std::move(rhs_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("IfByBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {100, 100, 100, 100, 100, 100, 100, 100, 100, 10, 11,  12, 13, 14, 15, 16,
                                17,  100, 19,  20,  21,  22,  23,  24,  100, 26, 100, 28, 29, 30, 31, 32};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, IfByBitmapTest3) {
  std::vector<uint8_t> bitmap = {0xFF, 0x1, 0x2, 0x5};
  std::vector<int8_t> lhs = {-1,  -2,  -3,  -4,  -5,  -6,  -7,  -8,  -9,  -10, -11, -12, -13, -14, -15, -16,
                             -17, -18, -19, -20, -21, -22, -23, -24, -25, -26, -27, -28, -29, -30, -31, -32};
  std::vector<int8_t> rhs = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                             17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};

  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto bitmap_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(bitmap));
  auto lhs_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(lhs));
  auto rhs_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(rhs));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(bitmap_node));
  args_list.emplace_back(std::move(lhs_node));
  args_list.emplace_back(std::move(rhs_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("IfByBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int8_t> expect = {-1, -2,  -3, -4, -5, -6, -7, -8, -9,  10, 11,  12, 13, 14, 15, 16,
                                17, -18, 19, 20, 21, 22, 23, 24, -25, 26, -27, 28, 29, 30, 31, 32};
  EXPECT_EQ(std::get<std::vector<int8_t>>(result), expect);
}

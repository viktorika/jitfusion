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

TEST(FunctionTest, ListAddTest1) {
  std::vector<uint8_t> data = {1, 2, 3, 4};
  uint8_t add = 100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto add_node = std::unique_ptr<ExecNode>(new ConstantValueNode(add));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(add_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListAdd", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {101, 102, 103, 104};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, ListAddTest2) {
  std::vector<int64_t> data = {1, 2, 3, 4};
  int64_t add = -100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto add_node = std::unique_ptr<ExecNode>(new ConstantValueNode(add));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(add_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListAdd", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int64_t> expect = {-99, -98, -97, -96};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expect);
}

TEST(FunctionTest, ListAddTest3) {
  std::vector<float> data = {1, 2, 3, 4};
  float add = -100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto add_node = std::unique_ptr<ExecNode>(new ConstantValueNode(add));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(add_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListAdd", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<float> expect = {-99, -98, -97, -96};
  EXPECT_EQ(std::get<std::vector<float>>(result), expect);
}

TEST(FunctionTest, ListAddTest4) {
  std::vector<uint8_t> data1 = {1, 2, 3, 4};
  std::vector<uint8_t> data2 = {5, 6, 7, 8};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto data1_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto data2_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(data1_node));
  args_list.emplace_back(std::move(data2_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListAdd", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint8_t> expect = {6, 8, 10, 12};
  EXPECT_EQ(std::get<std::vector<uint8_t>>(result), expect);
}

TEST(FunctionTest, ListAddTest5) {
  std::vector<int64_t> data1 = {1, 2, 3, 4};
  std::vector<int64_t> data2 = {5, 6, 7, 8};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto data1_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto data2_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(data1_node));
  args_list.emplace_back(std::move(data2_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListAdd", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int64_t> expect = {6, 8, 10, 12};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expect);
}

TEST(FunctionTest, ListAddTest6) {
  std::vector<float> data1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::vector<float> data2 = {-5, -6, -7, -8, -9, -10, -11, -12, -13, -14};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto data1_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto data2_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(data1_node));
  args_list.emplace_back(std::move(data2_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListAdd", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<float> expect = {-4, -4, -4, -4, -4, -4, -4, -4, -4, -4};
  EXPECT_EQ(std::get<std::vector<float>>(result), expect);
}

TEST(FunctionTest, ListSubTest1) {
  std::vector<int8_t> data = {1, 2, 3, 4};
  int8_t sub = -100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto sub_node = std::unique_ptr<ExecNode>(new ConstantValueNode(sub));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(sub_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListSub", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int8_t> expect = {101, 102, 103, 104};
  EXPECT_EQ(std::get<std::vector<int8_t>>(result), expect);
}

TEST(FunctionTest, ListSubTest2) {
  std::vector<uint32_t> data = {1, 2, 3, 4};
  uint32_t sub = -100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto sub_node = std::unique_ptr<ExecNode>(new ConstantValueNode(sub));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(sub_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListSub", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {101, 102, 103, 104};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, ListSubTest3) {
  std::vector<double> data = {1, 2, 3, 4};
  double sub = -100;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto sub_node = std::unique_ptr<ExecNode>(new ConstantValueNode(sub));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(sub_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListSub", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {101, 102, 103, 104};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, ListSubTest4) {
  std::vector<int8_t> data1 = {1, 2, 3, 4};
  std::vector<int8_t> data2 = {-100, -101, -102, -103};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto data1_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto data2_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(data1_node));
  args_list.emplace_back(std::move(data2_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListSub", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int8_t> expect = {101, 103, 105, 107};
  EXPECT_EQ(std::get<std::vector<int8_t>>(result), expect);
}

TEST(FunctionTest, ListSubTest5) {
  std::vector<uint32_t> data1 = {100, 200, 300, 400};
  std::vector<uint32_t> data2 = {10, 11, 12, 13};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto data1_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto data2_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(data1_node));
  args_list.emplace_back(std::move(data2_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListSub", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {90, 189, 288, 387};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, ListSubTest6) {
  std::vector<double> data1 = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100};
  std::vector<double> data2 = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto data1_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto data2_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(data1_node));
  args_list.emplace_back(std::move(data2_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListSub", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {90, 189, 288, 387, 486, 585, 684, 783, 882, 981, 1080};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, ListMulTest1) {
  std::vector<uint16_t> data = {1, 2, 3, 4};
  uint16_t mul = 10;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto mul_node = std::unique_ptr<ExecNode>(new ConstantValueNode(mul));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(mul_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMul", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint16_t> expect = {10, 20, 30, 40};
  EXPECT_EQ(std::get<std::vector<uint16_t>>(result), expect);
}

TEST(FunctionTest, ListMulTest2) {
  std::vector<int32_t> data = {1, 2, 3, 4};
  int32_t mul = 10;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto mul_node = std::unique_ptr<ExecNode>(new ConstantValueNode(mul));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(mul_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMul", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int32_t> expect = {10, 20, 30, 40};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expect);
}

TEST(FunctionTest, ListMulTest3) {
  std::vector<float> data = {1, 2, 3, 4};
  float mul = 10;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto mul_node = std::unique_ptr<ExecNode>(new ConstantValueNode(mul));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(mul_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMul", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<float> expect = {10, 20, 30, 40};
  EXPECT_EQ(std::get<std::vector<float>>(result), expect);
}

TEST(FunctionTest, ListMulTest4) {
  std::vector<uint16_t> data1 = {1, 2, 3, 4};
  std::vector<uint16_t> data2 = {10, 20, 30, 40};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto mul_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(mul_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMul", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint16_t> expect = {10, 40, 90, 160};
  EXPECT_EQ(std::get<std::vector<uint16_t>>(result), expect);
}

TEST(FunctionTest, ListMulTest5) {
  std::vector<int32_t> data1 = {1, 2, 3, 4};
  std::vector<int32_t> data2 = {10, 20, 30, 40};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto mul_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(mul_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMul", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int32_t> expect = {10, 40, 90, 160};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expect);
}

TEST(FunctionTest, ListMulTest6) {
  std::vector<float> data1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  std::vector<float> data2 = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto mul_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(mul_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMul", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<float> expect = {10, 40, 90, 160, 250, 360, 490, 640, 810, 1000, 1210};
  EXPECT_EQ(std::get<std::vector<float>>(result), expect);
}

TEST(FunctionTest, ListDivTest1) {
  std::vector<uint32_t> data = {1000, 2000, 3000, 4000};
  uint32_t div = 10;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto div_node = std::unique_ptr<ExecNode>(new ConstantValueNode(div));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(div_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListDiv", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {100, 200, 300, 400};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, ListDivTest2) {
  std::vector<int64_t> data = {1000, 2000, 3000, 4000};
  int64_t div = 10;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto div_node = std::unique_ptr<ExecNode>(new ConstantValueNode(div));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(div_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListDiv", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int64_t> expect = {100, 200, 300, 400};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expect);
}

TEST(FunctionTest, ListDivTest3) {
  std::vector<double> data = {1000, 2000, 3000, 4000};
  double div = 10;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto div_node = std::unique_ptr<ExecNode>(new ConstantValueNode(div));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(div_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListDiv", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {100, 200, 300, 400};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, ListDivTest4) {
  std::vector<uint32_t> data1 = {1000, 2000, 3000, 4000};
  std::vector<uint32_t> data2 = {10, 20, 30, 40};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto div_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(div_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListDiv", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {100, 100, 100, 100};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, ListDivTest5) {
  std::vector<int64_t> data1 = {1000, 2000, 3000, 4000};
  std::vector<int64_t> data2 = {10, 20, 30, 40};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto div_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(div_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListDiv", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int64_t> expect = {100, 100, 100, 100};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expect);
}

TEST(FunctionTest, ListDivTest6) {
  std::vector<double> data1 = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000};
  std::vector<double> data2 = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto div_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(div_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListDiv", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, ListModTest1) {
  std::vector<uint32_t> data = {1000, 2000, 3000, 4000};
  uint32_t mod = 7;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto mod_node = std::unique_ptr<ExecNode>(new ConstantValueNode(mod));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(mod_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMod", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {1000 % 7, 2000 % 7, 3000 % 7, 4000 % 7};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, ListModTest2) {
  std::vector<int64_t> data = {1000, 2000, 3000, 4000};
  int64_t mod = 7;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto mod_node = std::unique_ptr<ExecNode>(new ConstantValueNode(mod));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(mod_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMod", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int64_t> expect = {1000 % 7, 2000 % 7, 3000 % 7, 4000 % 7};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expect);
}

TEST(FunctionTest, ListModTest3) {
  std::vector<uint32_t> data1 = {1000, 2000, 3000, 4000};
  std::vector<uint32_t> data2 = {7, 8, 9, 10};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto mod_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(mod_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMod", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {1000 % 7, 2000 % 8, 3000 % 9, 4000 % 10};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, ListModTest4) {
  std::vector<int64_t> data1 = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000};
  std::vector<int64_t> data2 = {7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto mod_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(mod_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMod", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int64_t> expect = {1000 % 7,  2000 % 8,  3000 % 9,  4000 % 10,  5000 % 11, 6000 % 12,
                                 7000 % 13, 8000 % 14, 9000 % 15, 10000 % 16, 11000 % 17};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expect);
}

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

TEST(FunctionTest, ListExpTest1) {
  std::vector<int8_t> data = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListExp", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {std::exp(1), std::exp(2), std::exp(3), std::exp(4), std::exp(5),
                                std::exp(6), std::exp(7), std::exp(8), std::exp(9)};
  auto rs = std::get<std::vector<double>>(result);
  for (size_t i = 0; i < rs.size(); i++) {
    EXPECT_DOUBLE_EQ(rs[i], expect[i]);
  }
}

TEST(FunctionTest, ListExpTest2) {
  std::vector<uint64_t> data = {10, 20, 30, 40};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListExp", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {std::exp(10), std::exp(20), std::exp(30), std::exp(40)};
  auto rs = std::get<std::vector<double>>(result);
  for (size_t i = 0; i < rs.size(); i++) {
    EXPECT_DOUBLE_EQ(rs[i], expect[i]);
  }
}

TEST(FunctionTest, ListExpTest3) {
  std::vector<double> data = {100, 200, 300, 400};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListExp", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {std::exp(100), std::exp(200), std::exp(300), std::exp(400)};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, ListLogTest1) {
  std::vector<int16_t> data = {100, 200, 300, 400, 500, 600, 700, 800, 900};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListLog", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {std::log(100), std::log(200), std::log(300), std::log(400), std::log(500),
                                std::log(600), std::log(700), std::log(800), std::log(900)};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, ListLogTest2) {
  std::vector<uint64_t> data = {100, 200, 300, 400};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListLog", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {std::log(100), std::log(200), std::log(300), std::log(400)};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, ListLogTest3) {
  std::vector<float> data = {100.0F, 200.0F, 300.0F, 400.0F};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListLog", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<float> expect = {std::log(100.0F), std::log(200.0F), std::log(300.0F), std::log(400.0F)};
  EXPECT_EQ(std::get<std::vector<float>>(result), expect);
}

TEST(FunctionTest, ListLog2Test1) {
  std::vector<int16_t> data = {100, 200, 300, 400, 500, 600, 700, 800, 900};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListLog2", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {std::log2(100), std::log2(200), std::log2(300), std::log2(400), std::log2(500),
                                std::log2(600), std::log2(700), std::log2(800), std::log2(900)};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, ListLog2Test2) {
  std::vector<uint64_t> data = {100, 200, 300, 400};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListLog2", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {std::log2(100), std::log2(200), std::log2(300), std::log2(400)};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, ListLog2Test3) {
  std::vector<float> data = {100.0F, 200.0F, 300.0F, 400.0F};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListLog2", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<float> expect = {std::log2(100.0F), std::log2(200.0F), std::log2(300.0F), std::log2(400.0F)};
  EXPECT_EQ(std::get<std::vector<float>>(result), expect);
}

TEST(FunctionTest, ListLog10Test1) {
  std::vector<int16_t> data = {100, 200, 300, 400, 500, 600, 700, 800, 900};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListLog10", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {std::log10(100), std::log10(200), std::log10(300), std::log10(400), std::log10(500),
                                std::log10(600), std::log10(700), std::log10(800), std::log10(900)};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, ListLog10Test2) {
  std::vector<uint64_t> data = {100, 200, 300, 400};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListLog10", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {std::log10(100), std::log10(200), std::log10(300), std::log10(400)};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, ListLog10Test3) {
  std::vector<float> data = {100.0F, 200.0F, 300.0F, 400.0F};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListLog10", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<float> expect = {std::log10(100.0F), std::log10(200.0F), std::log10(300.0F), std::log10(400.0F)};
  EXPECT_EQ(std::get<std::vector<float>>(result), expect);
}

TEST(FunctionTest, ListCeilTest1) {
  std::vector<float> data = {100.5F, 200.5F, 300.5F, 400.5F, 500.5F, 600.5F, 700.5F, 800.5F, 900.5F};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListCeil", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<float> expect = {std::ceil(100.5F), std::ceil(200.5F), std::ceil(300.5F),
                               std::ceil(400.5F), std::ceil(500.5F), std::ceil(600.5F),
                               std::ceil(700.5F), std::ceil(800.5F), std::ceil(900.5F)};
  EXPECT_EQ(std::get<std::vector<float>>(result), expect);
}

TEST(FunctionTest, ListCeilTest2) {
  std::vector<double> data = {100.5F, 200.5F, 300.5F, 400.5F};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListCeil", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {std::ceil(100.5F), std::ceil(200.5F), std::ceil(300.5F), std::ceil(400.5F)};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, ListFloorTest1) {
  std::vector<float> data = {100.5F, 200.5F, 300.5F, 400.5F, 500.5F, 600.5F, 700.5F, 800.5F, 900.5F};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListFloor", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<float> expect = {std::floor(100.5F), std::floor(200.5F), std::floor(300.5F),
                               std::floor(400.5F), std::floor(500.5F), std::floor(600.5F),
                               std::floor(700.5F), std::floor(800.5F), std::floor(900.5F)};
  EXPECT_EQ(std::get<std::vector<float>>(result), expect);
}

TEST(FunctionTest, ListFloorTest2) {
  std::vector<double> data = {100.5F, 200.5F, 300.5F, 400.5F};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListFloor", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {std::floor(100.5F), std::floor(200.5F), std::floor(300.5F), std::floor(400.5F)};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, ListRoundTest1) {
  std::vector<float> data = {100.5F, 200.5F, 300.5F, 400.5F, 500.5F, 600.5F, 700.5F, 800.5F, 900.5F};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListRound", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<float> expect = {std::round(100.5F), std::round(200.5F), std::round(300.5F),
                               std::round(400.5F), std::round(500.5F), std::round(600.5F),
                               std::round(700.5F), std::round(800.5F), std::round(900.5F)};
  EXPECT_EQ(std::get<std::vector<float>>(result), expect);
}

TEST(FunctionTest, ListRoundTest2) {
  std::vector<double> data = {100.5F, 200.5F, 300.5F, 400.5F};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListRound", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {std::round(100.5F), std::round(200.5F), std::round(300.5F), std::round(400.5F)};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, ListMinTest1) {
  std::vector<uint16_t> data = {1, 2, 3, 4};
  uint16_t value = 2;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMin", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint16_t> expect = {1, 2, 2, 2};
  EXPECT_EQ(std::get<std::vector<uint16_t>>(result), expect);
}

TEST(FunctionTest, ListMinTest2) {
  std::vector<int32_t> data = {1, 2, 3, 4};
  int32_t value = 2;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMin", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int32_t> expect = {1, 2, 2, 2};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expect);
}

TEST(FunctionTest, ListMinTest3) {
  std::vector<float> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  float value = 2;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMin", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<float> expect = {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
  EXPECT_EQ(std::get<std::vector<float>>(result), expect);
}

TEST(FunctionTest, ListMaxTest1) {
  std::vector<uint16_t> data = {1, 2, 3, 4};
  uint16_t value = 2;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMax", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint16_t> expect = {2, 2, 3, 4};
  EXPECT_EQ(std::get<std::vector<uint16_t>>(result), expect);
}

TEST(FunctionTest, ListMaxTest2) {
  std::vector<int32_t> data = {1, 2, 3, 4};
  int32_t value = 2;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMax", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int32_t> expect = {2, 2, 3, 4};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expect);
}

TEST(FunctionTest, ListMaxTest3) {
  std::vector<float> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  float value = 2;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto value_node = std::unique_ptr<ExecNode>(new ConstantValueNode(value));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(value_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListMax", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<float> expect = {2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  EXPECT_EQ(std::get<std::vector<float>>(result), expect);
}

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

TEST(FunctionTest, ListBitwiseAndTest1) {
  std::vector<uint32_t> data = {1000, 2000, 3000, 4000};
  uint32_t and_data = 10;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto and_data_node = std::unique_ptr<ExecNode>(new ConstantValueNode(and_data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(and_data_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListBitwiseAnd", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {1000 & 10, 2000 & 10, 3000 & 10, 4000 & 10};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, ListBitwiseAndTest2) {
  std::vector<int64_t> data = {1000, 2000, 3000, 4000};
  int64_t and_data = 10;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto and_data_node = std::unique_ptr<ExecNode>(new ConstantValueNode(and_data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(and_data_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListBitwiseAnd", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int64_t> expect = {1000 & 10, 2000 & 10, 3000 & 10, 4000 & 10};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expect);
}

TEST(FunctionTest, ListBitwiseAndTest3) {
  std::vector<uint32_t> data1 = {1000, 2000, 3000, 4000};
  std::vector<uint32_t> data2 = {10, 20, 30, 40};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto data1_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto data2_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(data1_node));
  args_list.emplace_back(std::move(data2_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListBitwiseAnd", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {1000 & 10, 2000 & 20, 3000 & 30, 4000 & 40};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, ListBitwiseAndTest4) {
  std::vector<int64_t> data1 = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000};
  std::vector<int64_t> data2 = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto data1_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data1));
  auto data2_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data2));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(data1_node));
  args_list.emplace_back(std::move(data2_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListBitwiseAnd", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int64_t> expect = {1000 & 10, 2000 & 20, 3000 & 30, 4000 & 40,   5000 & 50,  6000 & 60,
                                 7000 & 70, 8000 & 80, 9000 & 90, 10000 & 100, 11000 & 110};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expect);
}

TEST(FunctionTest, ListBitwiseOrTest1) {
  std::vector<uint32_t> data = {1000, 2000, 3000, 4000};
  uint32_t or_data = 10;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto or_data_node = std::unique_ptr<ExecNode>(new ConstantValueNode(or_data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(or_data_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListBitwiseOr", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {1000 | 10, 2000 | 10, 3000 | 10, 4000 | 10};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, ListBitwiseOrTest2) {
  std::vector<int64_t> data = {1000, 2000, 3000, 4000};
  int64_t or_data = 10;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto or_data_node = std::unique_ptr<ExecNode>(new ConstantValueNode(or_data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(or_data_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListBitwiseOr", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int64_t> expect = {1000 | 10, 2000 | 10, 3000 | 10, 4000 | 10};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expect);
}

TEST(FunctionTest, ListBitwiseOrTest3) {
  std::vector<uint32_t> data = {1000, 2000, 3000, 4000};
  std::vector<uint32_t> or_data = {10, 10, 10, 10};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto or_data_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(or_data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(or_data_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListBitwiseOr", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {1000 | 10, 2000 | 10, 3000 | 10, 4000 | 10};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, ListBitwiseOrTest4) {
  std::vector<int64_t> data = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000};
  std::vector<int64_t> or_data = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto or_data_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(or_data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(or_data_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListBitwiseOr", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int64_t> expect = {1000 | 10, 2000 | 10, 3000 | 10, 4000 | 10,  5000 | 10, 6000 | 10,
                                 7000 | 10, 8000 | 10, 9000 | 10, 10000 | 10, 11000 | 10};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expect);
}

TEST(FunctionTest, ListBitwiseXorTest1) {
  std::vector<uint32_t> data = {1000, 2000, 3000, 4000};
  uint32_t xor_data = 10;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto xor_data_node = std::unique_ptr<ExecNode>(new ConstantValueNode(xor_data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(xor_data_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListBitwiseXor", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {1000 ^ 10, 2000 ^ 10, 3000 ^ 10, 4000 ^ 10};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, ListBitwiseXorTest2) {
  std::vector<int64_t> data = {1000, 2000, 3000, 4000};
  int64_t xor_data = 10;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto xor_data_node = std::unique_ptr<ExecNode>(new ConstantValueNode(xor_data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(xor_data_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListBitwiseXor", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int64_t> expect = {1000 ^ 10, 2000 ^ 10, 3000 ^ 10, 4000 ^ 10};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expect);
}

TEST(FunctionTest, ListBitwiseXorTest3) {
  std::vector<uint32_t> data = {1000, 2000, 3000, 4000};
  std::vector<uint32_t> xor_data = {10, 10, 10, 10};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto xor_data_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(xor_data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(xor_data_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListBitwiseXor", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {1000 ^ 10, 2000 ^ 10, 3000 ^ 10, 4000 ^ 10};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, ListBitwiseXorTest4) {
  std::vector<int64_t> data = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000};
  std::vector<int64_t> xor_data = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto xor_data_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(xor_data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(xor_data_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("ListBitwiseXor", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int64_t> expect = {1000 ^ 10, 2000 ^ 10, 3000 ^ 10, 4000 ^ 10,  5000 ^ 10, 6000 ^ 10,
                                 7000 ^ 10, 8000 ^ 10, 9000 ^ 10, 10000 ^ 10, 11000 ^ 10};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expect);
}

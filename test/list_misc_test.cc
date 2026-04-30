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

TEST(FunctionTest, FilterByBitmapTest1) {
  std::vector<int32_t> data = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                               17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
  std::vector<uint8_t> bitmap = {0xFF, 0x1, 0x2, 0x5};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto bitmap_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(bitmap));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(bitmap_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("FilterByBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int32_t> expect = {1, 2, 3, 4, 5, 6, 7, 8, 9, 18, 25, 27};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expect);
}

TEST(FunctionTest, FilterByBitmapTest2) {
  std::vector<double> data = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                              17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
  std::vector<uint8_t> bitmap = {0xFF, 0x1, 0x2, 0x5};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto bitmap_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(bitmap));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(bitmap_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("FilterByBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {1, 2, 3, 4, 5, 6, 7, 8, 9, 18, 25, 27};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

// Keep one test exercising the 4-arg explicit-bits_cnt kernel directly, so the
// underlying kernel (and its validation logic) stays under coverage even after
// the 3-arg sugar became the recommended path.
TEST(FunctionTest, FilterByBitmapExplicitBitsCntTest) {
  std::vector<int32_t> data = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                               17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
  std::vector<uint8_t> bitmap = {0xFF, 0x1, 0x2, 0x5};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto bitmap_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(bitmap));
  auto bits_cnt_node = std::unique_ptr<ExecNode>(new ConstantValueNode(12U));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(bitmap_node));
  args_list.emplace_back(std::move(bits_cnt_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("FilterByBitmap", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int32_t> expect = {1, 2, 3, 4, 5, 6, 7, 8, 9, 18, 25, 27};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expect);
}

// Sugar form where the bitmap comes from another function call (GenLargeBitmap)
// rather than a constant. Ensures that `CountBits` is emitted correctly over a
// non-constant bitmap SSA value and that CSE doesn't break semantics.
TEST(FunctionTest, FilterByBitmapSugarFromGenBitmapTest) {
  std::vector<int32_t> data = {0, 1, 2, 3, 4, 0, 1, 4, 9, 16};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());

  // bitmap = GenLargeBitmap(data, 3, exec_ctx)   -> keep elements > 3
  std::vector<std::unique_ptr<ExecNode>> gen_args;
  gen_args.emplace_back(new ConstantListValueNode(data));
  gen_args.emplace_back(new ConstantValueNode(static_cast<int32_t>(3)));
  gen_args.emplace_back(new ExecContextNode());
  auto bitmap_node = std::unique_ptr<ExecNode>(new FunctionNode("GenLargeBitmap", std::move(gen_args)));

  // FilterByBitmap(data, bitmap, exec_ctx) -- sugar, no bits_cnt.
  std::vector<std::unique_ptr<ExecNode>> filter_args;
  filter_args.emplace_back(new ConstantListValueNode(data));
  filter_args.emplace_back(std::move(bitmap_node));
  filter_args.emplace_back(new ExecContextNode());
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("FilterByBitmap", std::move(filter_args)));

  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int32_t> expect = {4, 4, 9, 16};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expect);
}

TEST(FunctionTest, MurmurHash3X8632Test1) {
  std::vector<int64_t> data = {1000, 2000, 3000, 4000};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("MurmurHash3X8632", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint32_t>(result), 1952695723U);
}

TEST(FunctionTest, GetAtU32Test) {
  std::vector<uint32_t> data = {10, 20, 30, 40, 50};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto index_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint32_t>(2)));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(index_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GetAt", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint32_t>(result), 30);
}

TEST(FunctionTest, GetAtI32Test) {
  std::vector<int32_t> data = {-10, -20, -30, -40};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto index_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint32_t>(3)));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(index_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GetAt", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), -40);
}

TEST(FunctionTest, GetAtF64Test) {
  std::vector<double> data = {1.1, 2.2, 3.3};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto index_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint32_t>(1)));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(index_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GetAt", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_DOUBLE_EQ(std::get<double>(result), 2.2);
}

TEST(FunctionTest, GetAtOutOfBoundsTest) {
  std::vector<int32_t> data = {100, 200, 300};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto index_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint32_t>(10)));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(index_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GetAt", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 0);
}

TEST(FunctionTest, GetAtFirstElementTest) {
  std::vector<uint64_t> data = {999, 888, 777};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto index_node = std::unique_ptr<ExecNode>(new ConstantValueNode(static_cast<uint32_t>(0)));
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(index_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("GetAt", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  EXPECT_EQ(std::get<uint64_t>(result), 999);
}

TEST(FunctionTest, UniqueU32Test) {
  std::vector<uint32_t> data = {3, 1, 2, 1, 3, 4, 2};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("Unique", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {3, 1, 2, 4};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, UniqueI32Test) {
  std::vector<int32_t> data = {-1, 2, -1, 3, 2, 3};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("Unique", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int32_t> expect = {-1, 2, 3};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expect);
}

TEST(FunctionTest, UniqueF64Test) {
  std::vector<double> data = {1.1, 2.2, 1.1, 3.3, 2.2};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("Unique", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<double> expect = {1.1, 2.2, 3.3};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(FunctionTest, UniqueAlreadyUniqueTest) {
  std::vector<uint32_t> data = {1, 2, 3, 4, 5};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("Unique", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect = {1, 2, 3, 4, 5};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(FunctionTest, UniqueAllSameTest) {
  std::vector<int64_t> data = {42, 42, 42, 42};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("Unique", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<int64_t> expect = {42};
  EXPECT_EQ(std::get<std::vector<int64_t>>(result), expect);
}

TEST(FunctionTest, UniqueStringTest) {
  std::vector<std::string> data = {"apple", "banana", "apple", "cherry", "banana", "date"};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("Unique", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<std::string> expect = {"apple", "banana", "cherry", "date"};
  EXPECT_EQ(std::get<std::vector<std::string>>(result), expect);
}

TEST(FunctionTest, UniqueStringAllSameTest) {
  std::vector<std::string> data = {"hello", "hello", "hello"};
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto args_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(data));
  auto exec_node = std::unique_ptr<ExecNode>(new ExecContextNode());
  std::vector<std::unique_ptr<ExecNode>> args_list;
  args_list.emplace_back(std::move(args_node));
  args_list.emplace_back(std::move(exec_node));
  auto op_node = std::unique_ptr<ExecNode>(new FunctionNode("Unique", std::move(args_list)));
  ExecEngine exec_engine;
  auto st = exec_engine.Compile(op_node, func_registry);
  ASSERT_TRUE(st.ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<std::string> expect = {"hello"};
  EXPECT_EQ(std::get<std::vector<std::string>>(result), expect);
}
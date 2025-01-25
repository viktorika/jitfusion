/*
 * @Author: victorika
 * @Date: 2025-01-24 10:40:25
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-24 14:36:23
 */
#include <memory>
#include "arena.h"
#include "exec_engine.h"
#include "exec_node.h"
#include "function_registry.h"
#include "gtest/gtest.h"
#include "status.h"
#include "type.h"

using namespace jitfusion;

namespace {

template <class T>
T LoadValue(int64_t data, uint32_t i) {
  return reinterpret_cast<uint32_t*>(data)[i];
}

// extern "C" int32_t LoadValue(int64_t data, int32_t i) { return 1; }

}  // namespace

TEST(EntryArgsTest, LoadDataTest) {
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load_i32", {ValueType::kI64, ValueType::kI32}, ValueType::kI32);
  FunctionStructure func_struct = {FunctionType::kCFunc, reinterpret_cast<void*>(&LoadValue<int32_t>), nullptr};
  func_registry->RegisterFunc(sign, func_struct);

  auto args_node = std::unique_ptr<ExecNode>(new EntryArgumentNode);
  auto index_node = std::unique_ptr<ExecNode>(new ConstantValueNode(1));
  std::vector<std::unique_ptr<ExecNode>> load_func_args;
  load_func_args.emplace_back(std::move(args_node));
  load_func_args.emplace_back(std::move(index_node));
  auto load_func_node = std::unique_ptr<ExecNode>(new FunctionNode("load_i32", std::move(load_func_args)));

  std::vector<int32_t> data = {1, 2, 3, 4};
  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.Compile(load_func_node, func_registry).ok());
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(data.data(), &result).ok());
  EXPECT_EQ(std::get<int32_t>(result), 2U);
}

GTEST_API_ int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
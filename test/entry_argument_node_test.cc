/*
 * @Author: victorika
 * @Date: 2025-01-24 10:40:25
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-24 14:36:23
 */
#include <memory>
#include "exec_engine.h"
#include "exec_node.h"
#include "function_registry.h"
#include "gtest/gtest.h"
#include "status.h"
#include "type.h"

using namespace jitfusion;

namespace {

template <class T>
T LoadValue(void* data, uint32_t i) {
  return reinterpret_cast<T*>(data)[i];
}

}  // namespace

TEST(EntryArgsTest, LoadDataTest) {
  std::vector<uint8_t> du8 = {1, 100};
  std::vector<uint16_t> du16 = {1, 30000};
  std::vector<uint32_t> du32 = {1, 1231231231};
  std::vector<uint64_t> du64 = {1, 324321413241321};
  std::vector<int8_t> ds8 = {1, -100};
  std::vector<int16_t> ds16 = {1, -30000};
  std::vector<int32_t> ds32 = {1, -1231231231};
  std::vector<int64_t> ds64 = {1, -324321413241321};
  std::vector<void*> data_list = {du8.data(), du16.data(), du32.data(), du64.data(),
                                  ds8.data(), ds16.data(), ds32.data(), ds64.data()};
  std::vector<ValueType> type_list = {ValueType::kU8, ValueType::kU16, ValueType::kU32, ValueType::kU64,
                                      ValueType::kI8, ValueType::kI16, ValueType::kI32, ValueType::kI64};
  std::vector<RetType> result_list = {
      LoadValue<uint8_t>(du8.data(), 1),   LoadValue<uint16_t>(du16.data(), 1), LoadValue<uint32_t>(du32.data(), 1),
      LoadValue<uint64_t>(du64.data(), 1), LoadValue<int8_t>(ds8.data(), 1),    LoadValue<int16_t>(ds16.data(), 1),
      LoadValue<int32_t>(ds32.data(), 1),  LoadValue<int64_t>(ds64.data(), 1),
  };
  std::vector<void*> func_list = {
      reinterpret_cast<void*>(LoadValue<uint8_t>),  reinterpret_cast<void*>(LoadValue<uint16_t>),
      reinterpret_cast<void*>(LoadValue<uint32_t>), reinterpret_cast<void*>(LoadValue<uint64_t>),
      reinterpret_cast<void*>(LoadValue<int8_t>),   reinterpret_cast<void*>(LoadValue<int16_t>),
      reinterpret_cast<void*>(LoadValue<int32_t>),  reinterpret_cast<void*>(LoadValue<int64_t>),
  };
  for (size_t i = 0; i < func_list.size(); i++) {
    std::unique_ptr<FunctionRegistry> func_registry;
    EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, type_list[i]);
    FunctionStructure func_struct = {FunctionType::kCFunc, func_list[i], nullptr};
    func_registry->RegisterFunc(sign, func_struct);

    auto args_node = std::unique_ptr<ExecNode>(new EntryArgumentNode);
    auto index_node = std::unique_ptr<ExecNode>(new ConstantValueNode(1));
    std::vector<std::unique_ptr<ExecNode>> load_func_args;
    load_func_args.emplace_back(std::move(args_node));
    load_func_args.emplace_back(std::move(index_node));
    auto load_func_node = std::unique_ptr<ExecNode>(new FunctionNode("load", std::move(load_func_args)));

    ExecEngine exec_engine;
    auto st = exec_engine.Compile(load_func_node, func_registry);
    ASSERT_TRUE(st.ok());
    RetType result;
    EXPECT_TRUE(exec_engine.Execute(data_list[i], &result).ok());
    EXPECT_EQ(result, result_list[i]);
  }
}
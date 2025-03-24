/*
 * @Author: victorika
 * @Date: 2025-01-24 10:40:25
 * @Last Modified by: viktorika
 * @Last Modified time: 2025-01-28 20:59:54
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

namespace {

U32ListStruct CreateU32List(void* ctx) {
  auto* exec_ctx = reinterpret_cast<ExecContext*>(ctx);
  U32ListStruct u32_list;
  u32_list.data = reinterpret_cast<uint32_t*>(exec_ctx->arena.Allocate(sizeof(uint32_t) * 4));
  u32_list.data[0] = 1;
  u32_list.data[1] = 2;
  u32_list.data[2] = 3;
  u32_list.data[3] = 4;
  u32_list.len = 4;
  return u32_list;
}

}  // namespace

TEST(ExecContextTest, CreateDataTest) {
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("create_u32_list", {ValueType::kPtr}, ValueType::kU32List);
  FunctionStructure func_struct = {FunctionType::kCFunc, reinterpret_cast<void*>(CreateU32List), nullptr};
  func_registry->RegisterFunc(sign, func_struct);

  auto args_node = std::unique_ptr<ExecNode>(new ExecContextNode);
  std::vector<std::unique_ptr<ExecNode>> create_func_args;
  create_func_args.emplace_back(std::move(args_node));
  auto create_func_node = std::unique_ptr<ExecNode>(new FunctionNode("create_u32_list", std::move(create_func_args)));

  ExecEngine exec_engine;
  auto st = exec_engine.Compile(create_func_node, func_registry);
  RetType result;
  EXPECT_TRUE(exec_engine.Execute(nullptr, &result).ok());
  std::vector<uint32_t> expect_result = {1, 2, 3, 4};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect_result);
}
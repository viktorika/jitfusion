/*
 * @Author: victorika
 * @Date: 2026-04-02 16:26:46
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-02 16:43:57
 */

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include "exec_engine.h"
#include "exec_node.h"
#include "function_registry.h"
#include "gtest/gtest.h"
#include "status.h"
#include "type.h"

using namespace jitfusion;

namespace {
std::unique_ptr<ExecNode> MakeListAddNode(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs) {
  auto l_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(lhs));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantListValueNode(rhs));
  auto ctx_node = std::unique_ptr<ExecNode>(new ExecContextNode);
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(l_node));
  args.emplace_back(std::move(r_node));
  args.emplace_back(std::move(ctx_node));
  return std::unique_ptr<ExecNode>(new FunctionNode("ListAdd", std::move(args)));
}

std::unique_ptr<ExecNode> MakeSimpleAddNode(uint32_t lhs, uint32_t rhs) {
  auto l_node = std::unique_ptr<ExecNode>(new ConstantValueNode(lhs));
  auto r_node = std::unique_ptr<ExecNode>(new ConstantValueNode(rhs));
  return std::unique_ptr<ExecNode>(new BinaryOPNode(BinaryOPType::kAdd, std::move(l_node), std::move(r_node)));
}

}  // namespace

TEST(ExecErrorTest, ExecuteReturnsRuntimeError) {
  std::unique_ptr<FunctionRegistry> func_registry;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());

  auto exec_node = MakeListAddNode({1, 2, 3}, {4, 5});

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.Compile(exec_node, func_registry).ok());

  RetType result;
  Status st = exec_engine.Execute(nullptr, &result);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("ListAddList"), std::string::npos);
  EXPECT_NE(st.ToString().find("len mismatch"), std::string::npos);
}

TEST(ExecErrorTest, ExecuteWithCtxReturnsRuntimeErrorAndClears) {
  std::unique_ptr<FunctionRegistry> func_registry;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());

  auto exec_node = MakeListAddNode({1, 2, 3}, {4, 5});

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.Compile(exec_node, func_registry).ok());

  ExecContext exec_ctx(4096);
  RetType result;
  Status st = exec_engine.Execute(exec_ctx, nullptr, &result);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("ListAddList"), std::string::npos);
  EXPECT_FALSE(exec_ctx.HasErrors());
}

TEST(ExecErrorTest, ExecuteAtReturnsRuntimeError) {
  std::unique_ptr<FunctionRegistry> func_registry;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());

  std::vector<std::unique_ptr<ExecNode>> nodes;
  nodes.emplace_back(MakeSimpleAddNode(1, 2));
  nodes.emplace_back(MakeListAddNode({1, 2, 3}, {4, 5}));

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.BatchCompile(nodes, func_registry).ok());

  RetType result0;
  Status st0 = exec_engine.ExecuteAt(0, nullptr, &result0);
  EXPECT_TRUE(st0.ok());
  EXPECT_EQ(std::get<uint32_t>(result0), 3U);

  RetType result1;
  Status st1 = exec_engine.ExecuteAt(1, nullptr, &result1);
  EXPECT_FALSE(st1.ok());
  EXPECT_NE(st1.ToString().find("ListAddList"), std::string::npos);
}

TEST(ExecErrorTest, ExecuteAtWithCtxReturnsRuntimeErrorAndClears) {
  std::unique_ptr<FunctionRegistry> func_registry;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());

  std::vector<std::unique_ptr<ExecNode>> nodes;
  nodes.emplace_back(MakeListAddNode({1, 2, 3}, {4, 5}));

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.BatchCompile(nodes, func_registry).ok());

  ExecContext exec_ctx(4096);
  RetType result;
  Status st = exec_engine.ExecuteAt(0, exec_ctx, nullptr, &result);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("ListAddList"), std::string::npos);
  EXPECT_FALSE(exec_ctx.HasErrors());
}

TEST(ExecErrorTest, ExecuteAtOutOfRange) {
  std::unique_ptr<FunctionRegistry> func_registry;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());

  std::vector<std::unique_ptr<ExecNode>> nodes;
  nodes.emplace_back(MakeSimpleAddNode(1, 2));

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.BatchCompile(nodes, func_registry).ok());

  RetType result;
  Status st = exec_engine.ExecuteAt(999, nullptr, &result);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("index out of range"), std::string::npos);
}

TEST(ExecErrorTest, ExecuteAllPartialFailureCollectsAllErrors) {
  std::unique_ptr<FunctionRegistry> func_registry;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());

  std::vector<std::unique_ptr<ExecNode>> nodes;
  nodes.emplace_back(MakeSimpleAddNode(10, 20));
  nodes.emplace_back(MakeListAddNode({1, 2, 3}, {4, 5}));
  nodes.emplace_back(MakeSimpleAddNode(100, 200));
  nodes.emplace_back(MakeListAddNode({1}, {4, 5, 6}));

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.BatchCompile(nodes, func_registry).ok());

  std::vector<RetType> results;
  Status st = exec_engine.ExecuteAll(nullptr, &results);

  EXPECT_FALSE(st.ok());

  EXPECT_EQ(results.size(), 4U);

  EXPECT_EQ(std::get<uint32_t>(results[0]), 30U);
  EXPECT_EQ(std::get<uint32_t>(results[2]), 300U);

  std::string err_msg = st.ToString();
  EXPECT_NE(err_msg.find("ListAddList"), std::string::npos);

  size_t first_pos = err_msg.find("ListAddList");
  EXPECT_NE(first_pos, std::string::npos);
  size_t second_pos = err_msg.find("ListAddList", first_pos + 1);
  EXPECT_NE(second_pos, std::string::npos);
}

TEST(ExecErrorTest, ExecuteAllWithCtxPartialFailure) {
  std::unique_ptr<FunctionRegistry> func_registry;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());

  std::vector<std::unique_ptr<ExecNode>> nodes;
  nodes.emplace_back(MakeSimpleAddNode(1, 2));
  nodes.emplace_back(MakeListAddNode({1, 2}, {4, 5, 6}));
  nodes.emplace_back(MakeSimpleAddNode(3, 4));

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.BatchCompile(nodes, func_registry).ok());

  ExecContext exec_ctx(4096);
  std::vector<RetType> results;
  Status st = exec_engine.ExecuteAll(exec_ctx, nullptr, &results);

  EXPECT_FALSE(st.ok());

  EXPECT_EQ(results.size(), 3U);
  EXPECT_EQ(std::get<uint32_t>(results[0]), 3U);
  EXPECT_EQ(std::get<uint32_t>(results[2]), 7U);

  EXPECT_FALSE(exec_ctx.HasErrors());
}

TEST(ExecErrorTest, MultipleExecuteCallsDoNotAccumulateErrors) {
  std::unique_ptr<FunctionRegistry> func_registry;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());

  auto exec_node = MakeListAddNode({1, 2, 3}, {4, 5});

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.Compile(exec_node, func_registry).ok());

  ExecContext exec_ctx(4096);

  RetType result1;
  Status st1 = exec_engine.Execute(exec_ctx, nullptr, &result1);
  EXPECT_FALSE(st1.ok());
  EXPECT_FALSE(exec_ctx.HasErrors());

  RetType result2;
  Status st2 = exec_engine.Execute(exec_ctx, nullptr, &result2);
  EXPECT_FALSE(st2.ok());
  EXPECT_FALSE(exec_ctx.HasErrors());

  EXPECT_EQ(st1.ToString(), st2.ToString());
}

TEST(ExecErrorTest, BatchCompileAndExecuteAllSuccess) {
  std::unique_ptr<FunctionRegistry> func_registry;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());

  std::vector<std::unique_ptr<ExecNode>> nodes;
  nodes.emplace_back(MakeSimpleAddNode(10, 20));
  nodes.emplace_back(MakeSimpleAddNode(100, 200));
  nodes.emplace_back(MakeSimpleAddNode(1000, 2000));

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.BatchCompile(nodes, func_registry).ok());

  std::vector<RetType> results;
  Status st = exec_engine.ExecuteAll(nullptr, &results);
  EXPECT_TRUE(st.ok());

  ASSERT_EQ(results.size(), 3U);
  EXPECT_EQ(std::get<uint32_t>(results[0]), 30U);
  EXPECT_EQ(std::get<uint32_t>(results[1]), 300U);
  EXPECT_EQ(std::get<uint32_t>(results[2]), 3000U);
}

TEST(ExecErrorTest, BatchCompileAndExecuteAllWithCtxSuccess) {
  std::unique_ptr<FunctionRegistry> func_registry;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());

  std::vector<std::unique_ptr<ExecNode>> nodes;
  nodes.emplace_back(MakeSimpleAddNode(5, 7));
  nodes.emplace_back(MakeSimpleAddNode(11, 13));

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.BatchCompile(nodes, func_registry).ok());

  ExecContext exec_ctx(4096);
  std::vector<RetType> results;
  Status st = exec_engine.ExecuteAll(exec_ctx, nullptr, &results);
  EXPECT_TRUE(st.ok());

  ASSERT_EQ(results.size(), 2U);
  EXPECT_EQ(std::get<uint32_t>(results[0]), 12U);
  EXPECT_EQ(std::get<uint32_t>(results[1]), 24U);

  std::vector<RetType> results2;
  Status st2 = exec_engine.ExecuteAll(exec_ctx, nullptr, &results2);
  EXPECT_TRUE(st2.ok());
  ASSERT_EQ(results2.size(), 2U);
  EXPECT_EQ(std::get<uint32_t>(results2[0]), 12U);
  EXPECT_EQ(std::get<uint32_t>(results2[1]), 24U);
}

TEST(ExecErrorTest, ExecContextNormalPathReuse) {
  std::unique_ptr<FunctionRegistry> func_registry;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());

  auto exec_node = MakeSimpleAddNode(42, 58);

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.Compile(exec_node, func_registry).ok());

  ExecContext exec_ctx(4096);

  RetType result1;
  Status st1 = exec_engine.Execute(exec_ctx, nullptr, &result1);
  EXPECT_TRUE(st1.ok());
  EXPECT_EQ(std::get<uint32_t>(result1), 100U);
  EXPECT_FALSE(exec_ctx.HasErrors());

  RetType result2;
  Status st2 = exec_engine.Execute(exec_ctx, nullptr, &result2);
  EXPECT_TRUE(st2.ok());
  EXPECT_EQ(std::get<uint32_t>(result2), 100U);
  EXPECT_FALSE(exec_ctx.HasErrors());
}

/*
 * @Author: victorika
 * @Date: 2026-04-20 10:36:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-20 10:36:00
 */
#include <cstdint>
#include <memory>
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

float LoadF32(void* entry_arguments, int32_t index) { return reinterpret_cast<float*>(entry_arguments)[index]; }

void StoreF32(void* output, int32_t index, float value) { reinterpret_cast<float*>(output)[index] = value; }

std::unique_ptr<ExecNode> MakeLoadF32(int32_t index) {
  auto entry_arg = std::unique_ptr<ExecNode>(new EntryArgumentNode);
  auto idx = std::unique_ptr<ExecNode>(new ConstantValueNode(index));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(entry_arg));
  args.emplace_back(std::move(idx));
  return std::unique_ptr<ExecNode>(new FunctionNode("load", std::move(args)));
}

std::unique_ptr<ExecNode> MakeStoreF32(int32_t index, std::unique_ptr<ExecNode> value) {
  auto output = std::unique_ptr<ExecNode>(new OutputNode);
  auto idx = std::unique_ptr<ExecNode>(new ConstantValueNode(index));
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(output));
  args.emplace_back(std::move(idx));
  args.emplace_back(std::move(value));
  return std::unique_ptr<ExecNode>(new FunctionNode("store", std::move(args)));
}

std::unique_ptr<ExecNode> MakeRef(const std::string& name) { return std::unique_ptr<ExecNode>(new RefNode(name)); }

std::unique_ptr<ExecNode> MakeConstF32(float val) { return std::unique_ptr<ExecNode>(new ConstantValueNode(val)); }

std::unique_ptr<ExecNode> MakeBinaryOP(BinaryOPType op, std::unique_ptr<ExecNode> left,
                                       std::unique_ptr<ExecNode> right) {
  return std::unique_ptr<ExecNode>(new BinaryOPNode(op, std::move(left), std::move(right)));
}

std::unique_ptr<FunctionRegistry> MakeRegistry() {
  std::unique_ptr<FunctionRegistry> func_registry;
  (void)FunctionRegistryFactory::CreateFunctionRegistry(&func_registry);
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    (void)func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32));
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    (void)func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1);
  }
  return func_registry;
}

}  // namespace

TEST(IfBlockTest, WhenElseModifyAndStore) {
  auto func_registry = MakeRegistry();

  auto load_a = MakeLoadF32(0);
  auto init_result = MakeConstF32(0.0F);

  {
    auto cond = MakeBinaryOP(BinaryOPType::kLarge, MakeRef("a"), MakeConstF32(0.0F));
    auto body_expr = MakeBinaryOP(BinaryOPType::kAdd, MakeRef("a"), MakeConstF32(1.0F));
    auto body_noop = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
    static_cast<NoOPNode*>(body_noop.get())->AppendArgs("result", std::move(body_expr));
    auto else_expr = MakeBinaryOP(BinaryOPType::kSub, MakeRef("a"), MakeConstF32(1.0F));
    auto else_noop = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
    static_cast<NoOPNode*>(else_noop.get())->AppendArgs("result", std::move(else_expr));
    std::vector<std::unique_ptr<ExecNode>> if_args;
    if_args.emplace_back(std::move(cond));
    if_args.emplace_back(std::move(body_noop));
    if_args.emplace_back(std::move(else_noop));
    auto if_block = std::unique_ptr<ExecNode>(new IfBlockNode(std::move(if_args)));
    auto store_node = MakeStoreF32(0, MakeRef("result"));
    auto root = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
    static_cast<NoOPNode*>(root.get())->AppendArgs("a", std::move(load_a));
    static_cast<NoOPNode*>(root.get())->AppendArgs("result", std::move(init_result));
    static_cast<NoOPNode*>(root.get())->AppendArgs(std::move(if_block));
    static_cast<NoOPNode*>(root.get())->AppendArgs(std::move(store_node));

    ExecEngine exec_engine;
    ASSERT_TRUE(exec_engine.Compile(root, func_registry).ok());
    {
      std::vector<float> value = {5.0F};
      std::vector<float> output = {0.0F};
      ASSERT_TRUE(exec_engine.Execute(value.data(), output.data()).ok());
      EXPECT_NEAR(output[0], 6.0F, 1e-5);
    }
    {
      std::vector<float> value = {-3.0F};
      std::vector<float> output = {0.0F};
      ASSERT_TRUE(exec_engine.Execute(value.data(), output.data()).ok());
      EXPECT_NEAR(output[0], -4.0F, 1e-5);
    }
  }
}

TEST(IfBlockTest, WhenElifElseModifyAndStore) {
  auto func_registry = MakeRegistry();

  auto load_a = MakeLoadF32(0);
  auto init_result = MakeConstF32(0.0f);

  auto cond1 = MakeBinaryOP(BinaryOPType::kLarge, MakeRef("a"), MakeConstF32(100.0F));
  auto body1 = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(body1.get())->AppendArgs("result", MakeConstF32(3.0F));

  auto cond2 = MakeBinaryOP(BinaryOPType::kLarge, MakeRef("a"), MakeConstF32(10.0F));
  auto body2 = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(body2.get())->AppendArgs("result", MakeConstF32(2.0F));

  auto else_body = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(else_body.get())->AppendArgs("result", MakeConstF32(1.0F));

  std::vector<std::unique_ptr<ExecNode>> if_args;
  if_args.emplace_back(std::move(cond1));
  if_args.emplace_back(std::move(body1));
  if_args.emplace_back(std::move(cond2));
  if_args.emplace_back(std::move(body2));
  if_args.emplace_back(std::move(else_body));
  auto if_block = std::unique_ptr<ExecNode>(new IfBlockNode(std::move(if_args)));

  auto store_node = MakeStoreF32(0, MakeRef("result"));

  auto root = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(root.get())->AppendArgs("a", std::move(load_a));
  static_cast<NoOPNode*>(root.get())->AppendArgs("result", std::move(init_result));
  static_cast<NoOPNode*>(root.get())->AppendArgs(std::move(if_block));
  static_cast<NoOPNode*>(root.get())->AppendArgs(std::move(store_node));

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.Compile(root, func_registry).ok());

  {
    std::vector<float> value = {200.0F};
    std::vector<float> output = {0.0F};
    ASSERT_TRUE(exec_engine.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 3.0F, 1e-5);
  }

  {
    std::vector<float> value = {50.0F};
    std::vector<float> output = {0.0F};
    ASSERT_TRUE(exec_engine.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 2.0F, 1e-5);
  }

  {
    std::vector<float> value = {5.0F};
    std::vector<float> output = {0.0F};
    ASSERT_TRUE(exec_engine.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 1.0F, 1e-5);
  }
}

TEST(IfBlockTest, WhenWithoutElsePartialModify) {
  auto func_registry = MakeRegistry();

  auto load_a = MakeLoadF32(0);
  auto init_result = MakeConstF32(99.0F);

  auto cond = MakeBinaryOP(BinaryOPType::kLarge, MakeRef("a"), MakeConstF32(10.0F));
  auto body = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(body.get())
      ->AppendArgs("result", MakeBinaryOP(BinaryOPType::kMul, MakeRef("a"), MakeConstF32(2.0F)));

  std::vector<std::unique_ptr<ExecNode>> if_args;
  if_args.emplace_back(std::move(cond));
  if_args.emplace_back(std::move(body));
  auto if_block = std::unique_ptr<ExecNode>(new IfBlockNode(std::move(if_args)));

  auto store_node = MakeStoreF32(0, MakeRef("result"));

  auto root = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(root.get())->AppendArgs("a", std::move(load_a));
  static_cast<NoOPNode*>(root.get())->AppendArgs("result", std::move(init_result));
  static_cast<NoOPNode*>(root.get())->AppendArgs(std::move(if_block));
  static_cast<NoOPNode*>(root.get())->AppendArgs(std::move(store_node));

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.Compile(root, func_registry).ok());

  {
    std::vector<float> value = {20.0F};
    std::vector<float> output = {0.0F};
    ASSERT_TRUE(exec_engine.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 40.0F, 1e-5);
  }

  {
    std::vector<float> value = {5.0F};
    std::vector<float> output = {0.0F};
    ASSERT_TRUE(exec_engine.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 99.0F, 1e-5);
  }
}

TEST(IfBlockTest, MultipleVarsModifyAndStore) {
  auto func_registry = MakeRegistry();

  auto load_a = MakeLoadF32(0);
  auto load_b = MakeLoadF32(1);
  auto init_r1 = MakeConstF32(0.0F);
  auto init_r2 = MakeConstF32(0.0F);

  auto cond = MakeBinaryOP(BinaryOPType::kLarge, MakeRef("a"), MakeConstF32(0.0F));

  auto body = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(body.get())->AppendArgs("r1", MakeBinaryOP(BinaryOPType::kAdd, MakeRef("a"), MakeRef("b")));
  static_cast<NoOPNode*>(body.get())->AppendArgs("r2", MakeBinaryOP(BinaryOPType::kMul, MakeRef("a"), MakeRef("b")));

  auto else_body = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(else_body.get())
      ->AppendArgs("r1", MakeBinaryOP(BinaryOPType::kSub, MakeRef("a"), MakeRef("b")));
  static_cast<NoOPNode*>(else_body.get())
      ->AppendArgs("r2", MakeBinaryOP(BinaryOPType::kDiv, MakeRef("a"), MakeRef("b")));

  std::vector<std::unique_ptr<ExecNode>> if_args;
  if_args.emplace_back(std::move(cond));
  if_args.emplace_back(std::move(body));
  if_args.emplace_back(std::move(else_body));
  auto if_block = std::unique_ptr<ExecNode>(new IfBlockNode(std::move(if_args)));

  auto store_r1 = MakeStoreF32(0, MakeRef("r1"));
  auto store_r2 = MakeStoreF32(1, MakeRef("r2"));

  auto root = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(root.get())->AppendArgs("a", std::move(load_a));
  static_cast<NoOPNode*>(root.get())->AppendArgs("b", std::move(load_b));
  static_cast<NoOPNode*>(root.get())->AppendArgs("r1", std::move(init_r1));
  static_cast<NoOPNode*>(root.get())->AppendArgs("r2", std::move(init_r2));
  static_cast<NoOPNode*>(root.get())->AppendArgs(std::move(if_block));
  static_cast<NoOPNode*>(root.get())->AppendArgs(std::move(store_r1));
  static_cast<NoOPNode*>(root.get())->AppendArgs(std::move(store_r2));

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.Compile(root, func_registry).ok());

  {
    std::vector<float> value = {5.0F, 3.0F};
    std::vector<float> output = {0.0F, 0.0F};
    ASSERT_TRUE(exec_engine.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 8.0F, 1e-5);
    EXPECT_NEAR(output[1], 15.0F, 1e-5);
  }

  {
    std::vector<float> value = {-4.0F, 2.0F};
    std::vector<float> output = {0.0F, 0.0F};
    ASSERT_TRUE(exec_engine.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -6.0F, 1e-5);
    EXPECT_NEAR(output[1], -2.0F, 1e-5);
  }
}

TEST(IfBlockTest, StoreInsideWhenBlock) {
  auto func_registry = MakeRegistry();

  auto load_a = MakeLoadF32(0);

  auto cond = MakeBinaryOP(BinaryOPType::kLarge, MakeRef("a"), MakeConstF32(0.0F));

  auto body = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(body.get())
      ->AppendArgs(MakeStoreF32(0, MakeBinaryOP(BinaryOPType::kMul, MakeRef("a"), MakeConstF32(2.0F))));
  auto else_body = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(else_body.get())
      ->AppendArgs(MakeStoreF32(0, MakeBinaryOP(BinaryOPType::kMul, MakeRef("a"), MakeConstF32(-1.0F))));

  std::vector<std::unique_ptr<ExecNode>> if_args;
  if_args.emplace_back(std::move(cond));
  if_args.emplace_back(std::move(body));
  if_args.emplace_back(std::move(else_body));
  auto if_block = std::unique_ptr<ExecNode>(new IfBlockNode(std::move(if_args)));

  auto root = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(root.get())->AppendArgs("a", std::move(load_a));
  static_cast<NoOPNode*>(root.get())->AppendArgs(std::move(if_block));

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.Compile(root, func_registry).ok());

  {
    std::vector<float> value = {5.0F};
    std::vector<float> output = {0.0F};
    ASSERT_TRUE(exec_engine.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 10.0F, 1e-5);
  }

  {
    std::vector<float> value = {-3.0F};
    std::vector<float> output = {0.0F};
    ASSERT_TRUE(exec_engine.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 3.0F, 1e-5);
  }
}

TEST(IfBlockTest, PartialModifyAndStore) {
  auto func_registry = MakeRegistry();

  auto load_a = MakeLoadF32(0);
  auto init_r1 = MakeConstF32(0.0F);
  auto init_r2 = MakeConstF32(100.0F);

  auto cond = MakeBinaryOP(BinaryOPType::kLarge, MakeRef("a"), MakeConstF32(0.0F));
  auto body = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(body.get())
      ->AppendArgs("r1", MakeBinaryOP(BinaryOPType::kAdd, MakeRef("a"), MakeConstF32(1.0F)));
  static_cast<NoOPNode*>(body.get())
      ->AppendArgs("r2", MakeBinaryOP(BinaryOPType::kAdd, MakeRef("a"), MakeConstF32(2.0F)));

  auto else_body = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(else_body.get())
      ->AppendArgs("r1", MakeBinaryOP(BinaryOPType::kSub, MakeRef("a"), MakeConstF32(1.0F)));

  std::vector<std::unique_ptr<ExecNode>> if_args;
  if_args.emplace_back(std::move(cond));
  if_args.emplace_back(std::move(body));
  if_args.emplace_back(std::move(else_body));
  auto if_block = std::unique_ptr<ExecNode>(new IfBlockNode(std::move(if_args)));

  auto store_r1 = MakeStoreF32(0, MakeRef("r1"));
  auto store_r2 = MakeStoreF32(1, MakeRef("r2"));

  auto root = std::unique_ptr<ExecNode>(new NoOPNode({}, {}));
  static_cast<NoOPNode*>(root.get())->AppendArgs("a", std::move(load_a));
  static_cast<NoOPNode*>(root.get())->AppendArgs("r1", std::move(init_r1));
  static_cast<NoOPNode*>(root.get())->AppendArgs("r2", std::move(init_r2));
  static_cast<NoOPNode*>(root.get())->AppendArgs(std::move(if_block));
  static_cast<NoOPNode*>(root.get())->AppendArgs(std::move(store_r1));
  static_cast<NoOPNode*>(root.get())->AppendArgs(std::move(store_r2));

  ExecEngine exec_engine;
  ASSERT_TRUE(exec_engine.Compile(root, func_registry).ok());

  {
    std::vector<float> value = {5.0F};
    std::vector<float> output = {0.0F, 0.0F};
    ASSERT_TRUE(exec_engine.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 6.0F, 1e-5);
    EXPECT_NEAR(output[1], 7.0F, 1e-5);
  }

  {
    std::vector<float> value = {-3.0F};
    std::vector<float> output = {0.0F, 0.0F};
    ASSERT_TRUE(exec_engine.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -4.0F, 1e-5);
    EXPECT_NEAR(output[1], 100.0F, 1e-5);
  }
}

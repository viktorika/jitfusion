#include "gtest/gtest.h"
#include "test_helper.h"

using athena::Athena;
using athena::FunctionRegistry;
using athena::FunctionRegistryFactory;
using athena::FunctionSignature;
using athena::RetType;
using athena::ValueType;
using test::LoadF32;
using test::LoadI32List;
using test::StoreF32;

TEST(RefNodeTest, BasicVariableReference) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = a + b;
  d = a - b;
  store(output, 0, c);
  store(output, 1, d);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {10, 3};
  std::vector<float> output = {0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 10 + 3);
  EXPECT_EQ(output[1], 10 - 3);
}

TEST(RefNodeTest, ChainedVariableReference) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = a + b;
  d = c * 2.0f32;
  e = c + d;
  store(output, 0, e);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {5, 3};
  std::vector<float> output = {0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 24);
}

TEST(RefNodeTest, MultipleReferencesToSameVariable) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code = R"(
  a = load(entry_arg, 0);
  b = a + a;
  c = a * a;
  d = b + c;
  store(output, 0, b);
  store(output, 1, c);
  store(output, 2, d);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {7};
  std::vector<float> output = {0, 0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 14);
  EXPECT_EQ(output[1], 49);
  EXPECT_EQ(output[2], 63);
}

TEST(RefNodeTest, RefNodeWithIfAndSwitch) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = if(a > b, a - b, b - a);
  d = switch(c > 10, c * 2.0f32, c > 5, c * 1.5f32, c);
  store(output, 0, c);
  store(output, 1, d);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {20, 5};
  std::vector<float> output = {0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 15);
  EXPECT_EQ(output[1], 30);
}

TEST(RefNodeTest, RefNodeWithFunctionCalls) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = max(a, b);
  d = min(a, b);
  e = abs(c - d);
  f = sqrt(CastF64(e));
  store(output, 0, CastF32(f));
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {25, 9};
  std::vector<float> output = {0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_NEAR(output[0], 4.0F, 1e-5);
}

TEST(RefNodeTest, MultiplePipelinesWithRefNode) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code1 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = a + b;
  d = c * c;
  store(output, 0, d);
  )";

  std::string code2 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = a - b;
  d = c * c;
  store(output, 1, d);
  )";

  std::string code3 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = a * b;
  d = c + c;
  store(output, 2, d);
  )";

  std::vector<std::string> codes = {code1, code2, code3};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {5, 3};
  std::vector<float> output = {0, 0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 64);
  EXPECT_EQ(output[1], 4);
  EXPECT_EQ(output[2], 30);
}

TEST(RefNodeTest, DeepChainReference) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code = R"(
  a = load(entry_arg, 0);
  b = a + 1.0f32;
  c = b + 1.0f32;
  d = c + 1.0f32;
  e = d + 1.0f32;
  f = e + 1.0f32;
  store(output, 0, f);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {10};
  std::vector<float> output = {0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 15);
}

TEST(RefNodeTest, RefNodeWithListOperations) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32List);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32List)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = ListAdd(a, b, exec_ctx);
  d = Len(c);
  e = Len(a) + Len(b);
  store(output, 0, CastF32(d));
  store(output, 1, CastF32(e));
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::array<std::vector<int32_t>, 2> value = {std::vector<int32_t>{1, 2, 3}, std::vector<int32_t>{4, 5, 6}};
  std::vector<float> output = {0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 3);
  EXPECT_EQ(output[1], 6);
}

TEST(RefNodeTest, ExpressionModeStillUsesClone) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = a + b;
  d = c * c;
  d;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {3, 4};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<float>(ret), 49);
}

TEST(RefNodeTest, ComplexPipelineWithLogicalAndRefNode) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code = R"(
  x = load(entry_arg, 0);
  y = load(entry_arg, 1);
  z = load(entry_arg, 2);
  sum = x + y + z;
  avg = sum / 3.0f32;
  diff = max(x, max(y, z)) - min(x, min(y, z));
  r1 = if(avg > 10.0f32, sum * 2.0f32, sum);
  r2 = switch(diff > 20.0f32, diff * 3.0f32, diff > 10.0f32, diff * 2.0f32, diff);
  store(output, 0, r1);
  store(output, 1, r2);
  store(output, 2, avg);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {10, 20, 30};
  std::vector<float> output = {0, 0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 120);
  EXPECT_EQ(output[1], 40);
  EXPECT_NEAR(output[2], 20.0F, 1e-5);
}

TEST(RefNodeTest, UndefinedVariableReferenceError) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code = R"(
  a = load(entry_arg, 0);
  b = a + undefined_var;
  store(output, 0, b);
  )";

  std::vector<std::string> codes = {code};
  auto status = athena.Compile(codes, func_registry);
  EXPECT_FALSE(status.ok());
  EXPECT_NE(status.ToString().find("Variable not found"), std::string::npos);
}

TEST(RefNodeTest, UndefinedVariableInExpressionMode) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }

  std::string code = R"(
  a = load(entry_arg, 0);
  nonexistent;
  )";

  auto status = athena.Compile(code, func_registry);
  EXPECT_FALSE(status.ok());
  EXPECT_NE(status.ToString().find("Variable not found"), std::string::npos);
}

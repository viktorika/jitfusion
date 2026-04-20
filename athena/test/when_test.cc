#include <cmath>

#include "gtest/gtest.h"
#include "test_helper.h"

using athena::Athena;
using athena::FunctionRegistry;
using athena::FunctionRegistryFactory;
using athena::FunctionSignature;
using athena::RetType;
using athena::ValueType;
using test::LoadF32;
using test::StoreF32;

TEST(WhenTest, IfElseModifyMultipleVars) {
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
  result1 = 0f32;
  result2 = 0f32;
  when a > 0 {
    result1 = a + b;
    result2 = a * b;
  } else {
    result1 = a - b;
    result2 = a / b;
  }
  store(output, 0, result1);
  store(output, 1, result2);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {5, 3};
    std::vector<float> output = {0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 8, 1e-5);
    EXPECT_NEAR(output[1], 15, 1e-5);
  }

  {
    std::vector<float> value = {-4, 2};
    std::vector<float> output = {0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -6, 1e-5);
    EXPECT_NEAR(output[1], -2, 1e-5);
  }
}

TEST(WhenTest, IfWithoutElse) {
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
  result = 99f32;
  when a > 10 {
    result = a * 2;
  }
  store(output, 0, result);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {20};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 40, 1e-5);
  }

  {
    std::vector<float> value = {5};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 99, 1e-5);
  }
}

TEST(WhenTest, IfElifElse) {
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
  result = 0f32;
  when a > 100 {
    result = 3f32;
  } elif a > 10 {
    result = 2f32;
  } else {
    result = 1f32;
  }
  store(output, 0, result);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {200};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 3, 1e-5);
  }

  {
    std::vector<float> value = {50};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 2, 1e-5);
  }

  {
    std::vector<float> value = {5};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 1, 1e-5);
  }
}

TEST(WhenTest, PartialModify) {
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
  r1 = 0f32;
  r2 = 100f32;
  when a > 0 {
    r1 = a + 1;
    r2 = a + 2;
  } else {
    r1 = a - 1;
  }
  store(output, 0, r1);
  store(output, 1, r2);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {5};
    std::vector<float> output = {0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 6, 1e-5);
    EXPECT_NEAR(output[1], 7, 1e-5);
  }

  {
    std::vector<float> value = {-3};
    std::vector<float> output = {0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -4, 1e-5);
    EXPECT_NEAR(output[1], 100, 1e-5);
  }
}

TEST(WhenTest, StoreInsideWhenElse) {
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
  when a > 0 {
    store(output, 0, a * 2.0f32);
  } else {
    store(output, 0, a * -1.0f32);
  }
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {5};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 10, 1e-5);
  }

  {
    std::vector<float> value = {-3};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 3, 1e-5);
  }
}

TEST(WhenTest, StoreInsideWhenElifElse) {
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
  when a > 100 {
    store(output, 0, 3.0f32);
  } elif a > 10 {
    store(output, 0, 2.0f32);
  } else {
    store(output, 0, 1.0f32);
  }
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {200};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 3, 1e-5);
  }

  {
    std::vector<float> value = {50};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 2, 1e-5);
  }

  {
    std::vector<float> value = {5};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 1, 1e-5);
  }
}

TEST(WhenTest, MixedModifyAndStoreInWhen) {
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
  result = 0f32;
  when a > 0 {
    result = a + b;
    store(output, 1, a * b);
  } else {
    result = a - b;
    store(output, 1, a / b);
  }
  store(output, 0, result);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {5, 3};
    std::vector<float> output = {0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 8, 1e-5);
    EXPECT_NEAR(output[1], 15, 1e-5);
  }

  {
    std::vector<float> value = {-4, 2};
    std::vector<float> output = {0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -6, 1e-5);
    EXPECT_NEAR(output[1], -2, 1e-5);
  }
}

TEST(WhenTest, StoreToMultipleOutputsInElifChain) {
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
  when a > 100 {
    store(output, 0, a + 1.0f32);
    store(output, 1, a + 2.0f32);
  } elif a > 10 {
    store(output, 0, a * 2.0f32);
    store(output, 1, a * 3.0f32);
  } else {
    store(output, 0, 0f32);
    store(output, 1, 0f32);
  }
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {200};
    std::vector<float> output = {0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 201, 1e-5);
    EXPECT_NEAR(output[1], 202, 1e-5);
  }

  {
    std::vector<float> value = {50};
    std::vector<float> output = {0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 100, 1e-5);
    EXPECT_NEAR(output[1], 150, 1e-5);
  }

  {
    std::vector<float> value = {5};
    std::vector<float> output = {0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 0, 1e-5);
    EXPECT_NEAR(output[1], 0, 1e-5);
  }
}

TEST(WhenTest, StoreInsideWhenWithoutElse) {
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
  store(output, 0, 99f32);
  when a > 10 {
    store(output, 0, a * 2.0f32);
  }
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {20};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 40, 1e-5);
  }

  {
    std::vector<float> value = {5};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 99, 1e-5);
  }
}

TEST(WhenTest, CompoundConditionsWithFunctions) {
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
  score = load(entry_arg, 0);
  level = load(entry_arg, 1);
  result = 0f32;
  when score > 90 and level >= 3 {
    result = score * 2.0f32;
  } elif score > 90 and level < 3 {
    result = score * 1.5f32;
  } elif score > 60 and level >= 3 {
    result = score * 1.2f32;
  } elif score > 60 or level >= 5 {
    result = score * 1.0f32;
  } else {
    result = score * 0.5f32;
  }
  store(output, 0, result);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());
  {
    std::vector<float> value = {95, 4};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 190, 1e-3);
  }
  {
    std::vector<float> value = {95, 2};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 142.5, 1e-3);
  }
  {
    std::vector<float> value = {75, 3};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 90, 1e-3);
  }
  {
    std::vector<float> value = {75, 1};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 75, 1e-3);
  }
  {
    std::vector<float> value = {30, 5};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 30, 1e-3);
  }
  {
    std::vector<float> value = {30, 1};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 15, 1e-3);
  }
}

TEST(WhenTest, IntermediateVarsAndComplexExprInWhen) {
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
  dist = CastF32(sqrt(CastF64(x * x + y * y)));
  category = 0f32;
  bonus = 0f32;
  when dist > 100f32 {
    category = 3f32;
    bonus = dist * 0.1f32;
  } elif dist > 50f32 {
    category = 2f32;
    bonus = dist * 0.05f32;
  } else {
    category = 1f32;
    bonus = 0f32;
  }
  store(output, 0, category);
  store(output, 1, bonus);
  store(output, 2, dist);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {60, 80};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    float dist = std::sqrt((60.0F * 60) + (80.0F * 80));
    EXPECT_NEAR(output[0], 2, 1e-5);
    EXPECT_NEAR(output[1], dist * 0.05f, 1e-2);
    EXPECT_NEAR(output[2], dist, 1e-2);
  }
  {
    std::vector<float> value = {80, 80};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    float dist = std::sqrt((80.0F * 80) + (80.0F * 80));
    EXPECT_NEAR(output[0], 3, 1e-5);
    EXPECT_NEAR(output[1], dist * 0.1F, 1e-2);
    EXPECT_NEAR(output[2], dist, 1e-2);
  }
  {
    std::vector<float> value = {3, 4};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 1, 1e-5);
    EXPECT_NEAR(output[1], 0, 1e-5);
    EXPECT_NEAR(output[2], 5, 1e-2);
  }
}

TEST(WhenTest, ConsecutiveWhenBlocks) {
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
  base = 0f32;
  when a > 100 {
    base = 10f32;
  } elif a > 50 {
    base = 5f32;
  } else {
    base = 1f32;
  }
  multiplier = 1f32;
  when b > 10 {
    multiplier = 3f32;
  } elif b > 5 {
    multiplier = 2f32;
  }
  store(output, 0, base * multiplier);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {200, 20};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 30, 1e-5);
  }
  {
    std::vector<float> value = {75, 7};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 10, 1e-5);
  }
  {
    std::vector<float> value = {30, 3};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 1, 1e-5);
  }
}

TEST(WhenTest, WhenMixedWithIfAndSwitchExpr) {
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
  result = 0f32;
  when a > 0 {
    coeff = if(b > 5, 2.0f32, 1.0f32);
    result = a * coeff;
  } else {
    penalty = switch(b > 10, 0.1f32, b > 5, 0.5f32, 1.0f32);
    result = a * penalty;
  }
  store(output, 0, result);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());
  {
    std::vector<float> value = {10, 8};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 20, 1e-5);
  }
  {
    std::vector<float> value = {10, 3};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 10, 1e-5);
  }
  {
    std::vector<float> value = {-10, 15};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -1, 1e-5);
  }
  {
    std::vector<float> value = {-10, 7};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -5, 1e-5);
  }
  {
    std::vector<float> value = {-10, 2};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -10, 1e-5);
  }
}

TEST(WhenTest, RateCalculationBusinessScenario) {
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
  amount = load(entry_arg, 0);
  type = load(entry_arg, 1);
  vip = load(entry_arg, 2);
  rate = 0f32;
  discount = 1f32;
  when type == 1 and vip >= 3 {
    rate = 0.01f32;
    discount = 0.8f32;
  } elif type == 1 and vip >= 1 {
    rate = 0.01f32;
    discount = 0.9f32;
  } elif type == 1 {
    rate = 0.01f32;
  } elif type == 2 and vip >= 3 {
    rate = 0.02f32;
    discount = 0.7f32;
  } elif type == 2 {
    rate = 0.02f32;
  } else {
    rate = 0.05f32;
  }
  fee = amount * rate * discount;
  final_fee = if(fee < 1f32, 1f32, fee);
  store(output, 0, final_fee);
  store(output, 1, rate);
  store(output, 2, discount);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {10000, 1, 3};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 80, 1e-2);
    EXPECT_NEAR(output[1], 0.01, 1e-5);
    EXPECT_NEAR(output[2], 0.8, 1e-5);
  }
  {
    std::vector<float> value = {10000, 1, 1};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 90, 1e-2);
    EXPECT_NEAR(output[1], 0.01, 1e-5);
    EXPECT_NEAR(output[2], 0.9, 1e-5);
  }
  {
    std::vector<float> value = {10000, 1, 0};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 100, 1e-2);
    EXPECT_NEAR(output[1], 0.01, 1e-5);
    EXPECT_NEAR(output[2], 1.0, 1e-5);
  }
  {
    std::vector<float> value = {10000, 2, 5};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 140, 1e-2);
    EXPECT_NEAR(output[1], 0.02, 1e-5);
    EXPECT_NEAR(output[2], 0.7, 1e-5);
  }
  {
    std::vector<float> value = {10000, 2, 0};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 200, 1e-2);
    EXPECT_NEAR(output[1], 0.02, 1e-5);
    EXPECT_NEAR(output[2], 1.0, 1e-5);
  }
  {
    std::vector<float> value = {10, 3, 0};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 1, 1e-5);
    EXPECT_NEAR(output[1], 0.05, 1e-5);
    EXPECT_NEAR(output[2], 1.0, 1e-5);
  }
}

TEST(WhenTest, FunctionCallInCondition) {
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
  diff = abs(a - b);
  avg = (a + b) / 2f32;
  result = 0f32;
  when diff > avg and max(a, b) > 100 {
    result = max(a, b) - min(a, b);
  } elif diff > avg {
    result = (max(a, b) - min(a, b)) / 2f32;
  } elif min(a, b) < 0 {
    result = abs(min(a, b));
  } else {
    result = avg;
  }
  store(output, 0, result);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {200, 10};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 190, 1e-3);
  }
  {
    std::vector<float> value = {50, 10};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 20, 1e-3);
  }
  {
    std::vector<float> value = {5, -10};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 7.5, 1e-3);
  }
  {
    std::vector<float> value = {10, 12};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 11, 1e-3);
  }
}

TEST(WhenTest, TwoLevelNesting) {
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
  result = 0f32;
  when a > 0 {
    when a > 100 {
      result = 3f32;
    } elif a > 10 {
      result = 2f32;
    } else {
      result = 1f32;
    }
  } else {
    when a < -100 {
      result = -3f32;
    } elif a < -10 {
      result = -2f32;
    } else {
      result = -1f32;
    }
  }
  store(output, 0, result);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {200};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 3, 1e-5);
  }
  {
    std::vector<float> value = {50};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 2, 1e-5);
  }
  {
    std::vector<float> value = {5};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 1, 1e-5);
  }
  {
    std::vector<float> value = {-200};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -3, 1e-5);
  }
  {
    std::vector<float> value = {-50};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -2, 1e-5);
  }
  {
    std::vector<float> value = {-5};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -1, 1e-5);
  }
}

TEST(WhenTest, TwoLevelWithStoreAndMultipleOutputs) {
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
  c = load(entry_arg, 2);
  r0 = 0f32;
  r1 = 0f32;
  when a > 0 and b > 0 {
    r0 = a + b;
    when c > 0 {
      r1 = c * 10f32;
      store(output, 2, 1f32);
    } else {
      r1 = 0f32;
      store(output, 2, 0f32);
    }
  } elif a > 0 {
    r0 = a;
    r1 = -1f32;
    store(output, 2, -1f32);
  } else {
    r0 = 0f32;
    r1 = 0f32;
    store(output, 2, -2f32);
  }
  store(output, 0, r0);
  store(output, 1, r1);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {5, 3, 2};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 8, 1e-5);
    EXPECT_NEAR(output[1], 20, 1e-5);
    EXPECT_NEAR(output[2], 1, 1e-5);
  }
  {
    std::vector<float> value = {5, 3, -1};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 8, 1e-5);
    EXPECT_NEAR(output[1], 0, 1e-5);
    EXPECT_NEAR(output[2], 0, 1e-5);
  }
  {
    std::vector<float> value = {5, -1, 10};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 5, 1e-5);
    EXPECT_NEAR(output[1], -1, 1e-5);
    EXPECT_NEAR(output[2], -1, 1e-5);
  }
  {
    std::vector<float> value = {-5, 3, 10};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 0, 1e-5);
    EXPECT_NEAR(output[1], 0, 1e-5);
    EXPECT_NEAR(output[2], -2, 1e-5);
  }
}

TEST(WhenTest, ThreeLevelNesting) {
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
  result = 0f32;
  when a > 0 {
    when a > 100 {
      when b > 0 {
        result = 100f32;
      } else {
        result = 99f32;
      }
    } else {
      when b > 0 {
        result = 50f32;
      } else {
        result = 49f32;
      }
    }
  } else {
    when a < -100 {
      when b > 0 {
        result = -100f32;
      } else {
        result = -99f32;
      }
    } else {
      when b > 0 {
        result = -50f32;
      } else {
        result = -49f32;
      }
    }
  }
  store(output, 0, result);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {200, 1};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 100, 1e-5);
  }
  {
    std::vector<float> value = {200, -1};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 99, 1e-5);
  }
  {
    std::vector<float> value = {50, 1};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 50, 1e-5);
  }
  {
    std::vector<float> value = {50, -1};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 49, 1e-5);
  }
  {
    std::vector<float> value = {-200, 1};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -100, 1e-5);
  }
  {
    std::vector<float> value = {-200, -1};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -99, 1e-5);
  }
  {
    std::vector<float> value = {-50, 1};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -50, 1e-5);
  }
  {
    std::vector<float> value = {-50, -1};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -49, 1e-5);
  }
}

TEST(WhenTest, FourLevelNesting) {
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
  result = 0f32;
  when a > 0 {
    when a > 50 {
      when b > 0 {
        when b > 50 {
          result = 1f32;
        } else {
          result = 2f32;
        }
      } else {
        result = 3f32;
      }
    } else {
      when b > 0 {
        when b > 50 {
          result = 4f32;
        } else {
          result = 5f32;
        }
      } else {
        result = 6f32;
      }
    }
  } else {
    when a < -50 {
      when b > 0 {
        result = 7f32;
      } else {
        result = 8f32;
      }
    } else {
      result = 9f32;
    }
  }
  store(output, 0, result);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {100, 100};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 1, 1e-5);
  }
  {
    std::vector<float> value = {100, 30};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 2, 1e-5);
  }
  {
    std::vector<float> value = {100, -10};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 3, 1e-5);
  }
  {
    std::vector<float> value = {30, 100};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 4, 1e-5);
  }
  {
    std::vector<float> value = {30, 30};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 5, 1e-5);
  }
  {
    std::vector<float> value = {30, -10};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 6, 1e-5);
  }
  {
    std::vector<float> value = {-100, 10};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 7, 1e-5);
  }
  {
    std::vector<float> value = {-100, -10};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 8, 1e-5);
  }
  {
    std::vector<float> value = {-30, 10};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 9, 1e-5);
  }
}

TEST(WhenTest, ThreeLevelModifyDifferentVars) {
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
  c = load(entry_arg, 2);
  tier = 0f32;
  grade = 0f32;
  bonus = 0f32;
  when a > 100 {
    tier = 3f32;
    when b > 50 {
      grade = 2f32;
      when c > 10 {
        bonus = 100f32;
      } else {
        bonus = 50f32;
      }
    } else {
      grade = 1f32;
      bonus = 10f32;
    }
  } elif a > 50 {
    tier = 2f32;
    when b > 50 {
      grade = 2f32;
      bonus = 30f32;
    } else {
      grade = 1f32;
      bonus = 5f32;
    }
  } else {
    tier = 1f32;
    grade = 0f32;
    bonus = 0f32;
  }
  store(output, 0, tier);
  store(output, 1, grade);
  store(output, 2, bonus);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {200, 80, 20};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 3, 1e-5);
    EXPECT_NEAR(output[1], 2, 1e-5);
    EXPECT_NEAR(output[2], 100, 1e-5);
  }
  {
    std::vector<float> value = {200, 80, 5};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 3, 1e-5);
    EXPECT_NEAR(output[1], 2, 1e-5);
    EXPECT_NEAR(output[2], 50, 1e-5);
  }
  {
    std::vector<float> value = {200, 30, 99};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 3, 1e-5);
    EXPECT_NEAR(output[1], 1, 1e-5);
    EXPECT_NEAR(output[2], 10, 1e-5);
  }
  {
    std::vector<float> value = {75, 80, 99};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 2, 1e-5);
    EXPECT_NEAR(output[1], 2, 1e-5);
    EXPECT_NEAR(output[2], 30, 1e-5);
  }
  {
    std::vector<float> value = {75, 30, 99};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 2, 1e-5);
    EXPECT_NEAR(output[1], 1, 1e-5);
    EXPECT_NEAR(output[2], 5, 1e-5);
  }
  {
    std::vector<float> value = {30, 99, 99};
    std::vector<float> output = {0, 0, 0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 1, 1e-5);
    EXPECT_NEAR(output[1], 0, 1e-5);
    EXPECT_NEAR(output[2], 0, 1e-5);
  }
}

TEST(WhenTest, ThreeLevelWithExpressions) {
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
  c = load(entry_arg, 2);
  result = 0f32;
  when a > 0 {
    factor = if(a > 100, 2.0f32, 1.0f32);
    when b > 0 {
      scale = switch(b > 100, 3.0f32, b > 50, 2.0f32, 1.0f32);
      when c > 0 {
        result = a * factor + b * scale + c;
      } else {
        result = a * factor + b * scale;
      }
    } else {
      result = a * factor;
    }
  } else {
    result = -1f32;
  }
  store(output, 0, result);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  {
    std::vector<float> value = {200, 80, 5};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 565, 1e-3);
  }
  {
    std::vector<float> value = {50, 30, -1};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 80, 1e-3);
  }
  {
    std::vector<float> value = {50, -10, 99};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], 50, 1e-3);
  }
  {
    std::vector<float> value = {-10, 99, 99};
    std::vector<float> output = {0};
    ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
    EXPECT_NEAR(output[0], -1, 1e-5);
  }
}

TEST(WhenTest, WhenBlockNotAllowedInExpressionMode) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }

  std::string code = R"(
  a = load(entry_arg, 0);
  r = 0f32;
  when a > 0 {
    r = 1f32;
  }
  r;
  )";

  auto status = athena.Compile(code, func_registry);
  EXPECT_FALSE(status.ok());
  EXPECT_NE(status.ToString().find("when block is not allowed in expression mode"), std::string::npos);
}

TEST(WhenTest, VariableTypeMismatchInWhenBranch) {
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
  r = 0.0f32;
  when a > 5.0f32 {
    r = 42;
  }
  store(output, 0, r);
  )";

  std::vector<std::string> codes = {code};
  auto status = athena.Compile(codes, func_registry);
  EXPECT_FALSE(status.ok());
  EXPECT_NE(status.ToString().find("type mismatch"), std::string::npos);
}

TEST(WhenTest, VariableTypeMismatchInElseBranch) {
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
  r = 0.0f32;
  when a > 5.0f32 {
    r = 1.0f32;
  } else {
    r = 42;
  }
  store(output, 0, r);
  )";

  std::vector<std::string> codes = {code};
  auto status = athena.Compile(codes, func_registry);
  EXPECT_FALSE(status.ok());
  EXPECT_NE(status.ToString().find("type mismatch"), std::string::npos);
}

TEST(WhenTest, StringConditionReject) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code = R"(
  r = 0.0f32;
  when "hello" {
    r = 1.0f32;
  }
  store(output, 0, r);
  )";

  std::vector<std::string> codes = {code};
  auto status = athena.Compile(codes, func_registry);
  EXPECT_FALSE(status.ok());
  EXPECT_NE(status.ToString().find("numeric type"), std::string::npos);
}

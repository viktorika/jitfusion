/*
 * @Author: victorika
 * @Date: 2026-07-15 10:30:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-07-15 10:30:00
 */
#include "gtest/gtest.h"
#include "test_helper.h"

using athena::AthenaExpression;
using athena::AthenaPipeline;
using athena::FunctionRegistry;
using athena::FunctionRegistryFactory;
using athena::FunctionSignature;
using athena::RetType;
using athena::ValueType;
using test::LoadF32;
using test::StoreF32;

TEST(ConstVarTest, BasicLetDeclaration) {
  AthenaPipeline athena;
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
  let a = load(entry_arg, 0);
  let b = load(entry_arg, 1);
  let c = a + b;
  store(output, 0, c);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {10, 3};
  std::vector<float> output = {0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 13.0F);
}

TEST(ConstVarTest, ReassignConstVarShouldFail) {
  AthenaPipeline athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }

  std::string code = R"(
  let a = load(entry_arg, 0);
  a = load(entry_arg, 1);
  )";

  std::vector<std::string> codes = {code};
  auto st = athena.Compile(codes, func_registry);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("cannot reassign const variable 'a'"), std::string::npos) << st.ToString();
}

TEST(ConstVarTest, ReassignMutableVarShouldSucceed) {
  AthenaPipeline athena;
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
  a = a + 1.0f32;
  store(output, 0, a);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {10};
  std::vector<float> output = {0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 11.0F);
}

TEST(ConstVarTest, ModifyConstVarInWhenShouldFail) {
  AthenaPipeline athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }

  std::string code = R"(
  let a = load(entry_arg, 0);
  when a > 0.0f32 {
    a = 1.0f32;
  }
  )";

  std::vector<std::string> codes = {code};
  auto st = athena.Compile(codes, func_registry);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("cannot reassign const variable 'a'"), std::string::npos) << st.ToString();
}

TEST(ConstVarTest, MixedConstAndMutableVars) {
  AthenaPipeline athena;
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
  let a = load(entry_arg, 0);
  b = a + 1.0f32;
  b = b * 2.0f32;
  store(output, 0, b);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {5};
  std::vector<float> output = {0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 12.0F);  // (5 + 1) * 2 = 12
}

TEST(ConstVarTest, ModifyMutableVarInWhenShouldSucceed) {
  AthenaPipeline athena;
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
  let a = load(entry_arg, 0);
  b = 0.0f32;
  when a > 0.0f32 {
    b = a + 1.0f32;
  } else {
    b = a - 1.0f32;
  }
  store(output, 0, b);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {5};
  std::vector<float> output = {0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 6.0F);
}

TEST(ConstVarTest, LetRedeclareExistingMutableVar) {
  AthenaPipeline athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }

  std::string code = R"(
  a = load(entry_arg, 0);
  let a = a + 1.0f32;
  )";

  std::vector<std::string> codes = {code};
  auto st = athena.Compile(codes, func_registry);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("cannot redeclare variable 'a' with let"), std::string::npos) << st.ToString();
}

TEST(ConstVarTest, LetRedeclareThenReassignShouldFail) {
  AthenaPipeline athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }

  std::string code = R"(
  a = load(entry_arg, 0);
  let a = a + 1.0f32;
  a = a + 2.0f32;
  )";

  std::vector<std::string> codes = {code};
  auto st = athena.Compile(codes, func_registry);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("cannot redeclare variable 'a' with let"), std::string::npos) << st.ToString();
}

TEST(ConstVarTest, NestedWhenThirdLayerReadFirstLayerConst) {
  AthenaPipeline athena;
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
  let a = load(entry_arg, 0);
  b = 0.0f32;
  when a > 0.0f32 {
    when a > 5.0f32 {
      when a > 10.0f32 {
        b = a + 100.0f32;
      }
    }
  }
  store(output, 0, b);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {15};
  std::vector<float> output = {0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 115.0F);  // 15 + 100 = 115
}

TEST(ConstVarTest, NestedWhenThirdLayerReadSecondLayerConst) {
  AthenaPipeline athena;
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
  let a = load(entry_arg, 0);
  b = 0.0f32;
  when a > 0.0f32 {
    let c = a * 2.0f32;
    when c > 10.0f32 {
      when c > 20.0f32 {
        b = c + 1.0f32;
      }
    }
  }
  store(output, 0, b);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {15};
  std::vector<float> output = {0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 31.0F);  // 15 * 2 + 1 = 31
}

TEST(ConstVarTest, NestedWhenSecondLayerReadFirstLayerConst) {
  AthenaPipeline athena;
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
  let a = load(entry_arg, 0);
  let b = load(entry_arg, 1);
  result = 0.0f32;
  when a > 0.0f32 {
    let c = a + b;
    when c > 10.0f32 {
      result = c * a;
    }
  }
  store(output, 0, result);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {5, 8};
  std::vector<float> output = {0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 65.0F);  // c = 5 + 8 = 13, result = 13 * 5 = 65
}

TEST(ConstVarTest, NestedWhenThirdLayerModifyFirstLayerConstShouldFail) {
  AthenaPipeline athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }

  std::string code = R"(
  let a = load(entry_arg, 0);
  when a > 0.0f32 {
    when a > 5.0f32 {
      when a > 10.0f32 {
        a = 999.0f32;
      }
    }
  }
  )";

  std::vector<std::string> codes = {code};
  auto st = athena.Compile(codes, func_registry);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("cannot reassign const variable 'a'"), std::string::npos) << st.ToString();
}

TEST(ConstVarTest, NestedWhenThirdLayerModifySecondLayerConstShouldFail) {
  AthenaPipeline athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }

  std::string code = R"(
  let a = load(entry_arg, 0);
  when a > 0.0f32 {
    let b = a + 1.0f32;
    when b > 5.0f32 {
      when b > 10.0f32 {
        b = 999.0f32;
      }
    }
  }
  )";

  std::vector<std::string> codes = {code};
  auto st = athena.Compile(codes, func_registry);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("cannot reassign const variable 'b'"), std::string::npos) << st.ToString();
}

TEST(ConstVarTest, NestedWhenSecondLayerModifyFirstLayerConstShouldFail) {
  AthenaPipeline athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }

  std::string code = R"(
  let a = load(entry_arg, 0);
  when a > 0.0f32 {
    when a > 5.0f32 {
      a = 999.0f32;
    }
  }
  )";

  std::vector<std::string> codes = {code};
  auto st = athena.Compile(codes, func_registry);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("cannot reassign const variable 'a'"), std::string::npos) << st.ToString();
}

TEST(ConstVarTest, NestedWhenMixedConstAndMutableComplex) {
  AthenaPipeline athena;
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
  let a = load(entry_arg, 0);
  result = 0.0f32;
  when a > 0.0f32 {
    let b = a * 3.0f32;
    result = b;
    when b > 10.0f32 {
      let c = a + b;
      when c > 50.0f32 {
        result = c + a + b;
      }
    }
  }
  store(output, 0, result);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {20};
  std::vector<float> output = {0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 160.0F);
}

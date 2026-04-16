#include "gtest/gtest.h"
#include "test_helper.h"

using athena::Athena;
using athena::FunctionRegistry;
using athena::FunctionRegistryFactory;
using athena::FunctionSignature;
using athena::RetType;
using athena::ValueType;
using test::LoadF32;

TEST(SwitchTest, Test1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  v1 = load(entry_arg, 0);
  v2 = load(entry_arg, 1);
  r = switch(0, 0.1, 0, v2, 0.5);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {0.1, 0.2};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), 0.5, 1e-5);
}

TEST(SwitchTest, Test2) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  s = load(entry_arg, 0);
  r = switch ((1<=CastI32(floor(s/100))%100 and CastI32(floor(s/100)%100)<=10), 1, 
               CastI32(floor(s/100))%100==11, 2, 6);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {9.1};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<int>(ret), 6);
}

TEST(SwitchTest, Test3) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  s = load(entry_arg, 0);
  s9 = load(entry_arg, 1);
  s21 = load(entry_arg, 2);
  s22 = load(entry_arg, 3);
  r = switch(s21==10 and (s22==1004070 or s22==1003073) and s9==2, s*1.8, s21==10 and (s22==1004070 or s22==1003073) and s9==1,
            s*1.4, (s21==10 and s22==1004116 and s9==1), s*1.5, (s21 != 10 and s9==3), s*1.8, s);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {12, 321, 31, 0.9};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), 12, 1e-5);
}

TEST(SwitchTest, Test4) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = switch(0,'a',1, 'b','c');
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<std::string>(ret), "b");
}

TEST(NestedIfTest, Test1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = if(a > 10, if(b > 5, a + b, a - b), a * b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {20, 8};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_NEAR(std::get<float>(ret), 28, 1e-5);
}

TEST(NestedIfTest, Test2) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = if(a > 10, a + b, if(b > 5, a * b, a / b));
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {3, 8};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_NEAR(std::get<float>(ret), 24, 1e-5);
}

TEST(NestedIfTest, Test3) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = load(entry_arg, 2);
  r = if(a > 10, if(b > 5, if(c > 1, a + b + c, a + b - c), a - b), a * b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {20, 8, 3};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_NEAR(std::get<float>(ret), 31, 1e-5);
}

TEST(NestedIfTest, Test4) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = if(a > 10, switch(b > 20, a + b, b > 10, a * b, a - b), a / b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {20, 15};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_NEAR(std::get<float>(ret), 300, 1e-5);
}

TEST(NestedSwitchTest, Test1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = switch(a > 100, a + b, a > 10, if(b > 5, a * b, a / b), a - b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {20, 8};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_NEAR(std::get<float>(ret), 160, 1e-5);
}

TEST(NestedSwitchTest, Test2) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = switch(a > 100, a + b, a > 50, a * b, if(b > 5, a - b, a / b));
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {3, 2};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_NEAR(std::get<float>(ret), 1.5, 1e-5);
}

TEST(NestedSwitchTest, Test3) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = switch(a > 100, a + b, a > 10, switch(b > 20, a * b, b > 5, a - b, a / b), 0);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {20, 8};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_NEAR(std::get<float>(ret), 12, 1e-5);
}

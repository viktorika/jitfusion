#include "gtest/gtest.h"
#include "test_helper.h"

using athena::Athena;
using athena::FunctionRegistry;
using athena::FunctionRegistryFactory;
using athena::FunctionSignature;
using athena::RetType;
using athena::ValueType;
using test::LoadF32;
using test::LoadI32;

TEST(LogicalOpTest, ChainedAnd) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = 1 and 2 and 3;)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(LogicalOpTest, ChainedAndShortCircuit) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = 1 and 0 and 3;)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(LogicalOpTest, ChainedOrAllFalse) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = 0 or 0 or 0;)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(LogicalOpTest, ChainedOrShortCircuit) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = 0 or 5 or 0;)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(LogicalOpTest, MixedAndOr1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = (1 and 0) or (1 and 1);)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(LogicalOpTest, MixedAndOr2) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = (0 or 1) and (0 or 0);)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(LogicalOpTest, LogicalWithArithmetic) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = (a + b > 10) and (a * b < 100);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {8, 5};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(LogicalOpTest, LogicalWithArithmeticFalse) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = (a + b > 100) and (a * b < 100);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {8, 5};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(LogicalOpTest, LogicalWithNot) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = not (1 and 0) or 0;)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(LogicalOpTest, DeepNestedLogical) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = ((1 and 1) or (0 and 1)) and (0 or 1);)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(LogicalOpTest, DeepNestedLogicalAllShortCircuit) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = ((0 and 1) or (0 and 1)) and (1 or 0);)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(LogicalOpTest, FloatChainedLogical) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = 1.5 and 2.5 and 0.0;)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(LogicalOpTest, FloatChainedOr) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = 0.0 or 0.0 or 3.14;)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(LogicalOpTest, LogicalResultInArithmetic) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = (a > 0 and b > 0) + (a < 0 or b < 0);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<int32_t> value = {5, 3};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), 1);
}

TEST(LogicalOpTest, LogicalInIfCondition) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = if(a > 0 and b > 0, a + b, a - b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {10, 20};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<float>(ret), 30);
}

TEST(LogicalOpTest, LogicalInIfConditionFalse) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = if(a > 100 and b > 0, a + b, a * b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {10, 20};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<float>(ret), 200);
}

TEST(LogicalOpTest, LogicalInSwitch) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = switch(a > 10 and b > 10, a + b, a > 10 or b > 10, a * b, a - b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {20, 5};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_NEAR(std::get<float>(ret), 100, 1e-5);
}

TEST(LogicalOpTest, ComplexMultiVarLogical) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = load(entry_arg, 2);
  d = load(entry_arg, 3);
  x = (a > 0 and b > 0);
  y = (c > 0 or d > 0);
  z = (a == b and c != d);
  r = if(x and y, if(z, a + b + c + d, a * b), c * d);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<int32_t> value = {5, 5, 3, -1};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), 12);
}

TEST(LogicalOpTest, ComplexMultiVarLogical2) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = load(entry_arg, 2);
  d = load(entry_arg, 3);
  x = (a > 0 and b > 0);
  y = (c > 0 or d > 0);
  z = (a == b and c != d);
  r = if(x and y, if(z, a + b + c + d, a * b), c * d);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<int32_t> value = {-1, 5, -3, -7};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), 21);
}

TEST(LogicalOpTest, LongChainComparison) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = load(entry_arg, 2);
  d = load(entry_arg, 3);
  r = a > b and b > c and c > d and d > 0;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<int32_t> value = {10, 7, 3, 1};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(LogicalOpTest, LongChainComparisonBreak) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = load(entry_arg, 2);
  d = load(entry_arg, 3);
  r = a > b and b > c and c > d and d > 0;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<int32_t> value = {10, 7, 30, 1};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(LogicalOpTest, LongChainOr) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = load(entry_arg, 2);
  d = load(entry_arg, 3);
  r = a == 1 or b == 2 or c == 3 or d == 4;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<int32_t> value = {99, 99, 99, 4};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(LogicalOpTest, LongChainOrAllFalse) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = load(entry_arg, 2);
  d = load(entry_arg, 3);
  r = a == 1 or b == 2 or c == 3 or d == 4;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<int32_t> value = {99, 99, 99, 99};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(LogicalOpTest, LogicalWithFunctionCalls) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = (abs(a) > 5 and abs(b) > 5) or (max(a, b) > 100);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {-10, 8};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(LogicalOpTest, LogicalInSwitchMultiConditions) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = switch(
    a > 100 and b > 100, 1,
    a > 50 or b > 50, 2,
    (a > 10 and b > 10) or (a + b > 30), 3,
    4
  );
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {20, 15};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), 3);
}

TEST(LogicalOpTest, StringAndReject) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto st = athena.Compile(R"(r = "hello" and "world";)", func_registry);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("logical operator requires numeric operands"), std::string::npos);
}

TEST(LogicalOpTest, StringOrReject) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto st = athena.Compile(R"(r = "hello" or "world";)", func_registry);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("logical operator requires numeric operands"), std::string::npos);
}

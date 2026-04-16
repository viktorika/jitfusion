#include "gtest/gtest.h"
#include "test_helper.h"

using athena::Athena;
using athena::FunctionRegistry;
using athena::FunctionRegistryFactory;
using athena::FunctionSignature;
using athena::RetType;
using athena::ValueType;
using test::LoadF32;
using test::LoadU32;

TEST(UnaryOperatorTest, Test1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = -42;)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), -42);
}

TEST(UnaryOperatorTest, Test2) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = ~42;)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), ~42);
}

TEST(UnaryOperatorTest, Test3) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = not 99;)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), uint8_t(0));
}

TEST(UnaryOperatorTest, Test4) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = !0;)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), 1);
}

TEST(BinaryOperatorTest, Test1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = 0 and 1;)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(BinaryOperatorTest, Test2) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = 0 || 1;)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(BinaryOperatorTest, Test3) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kU32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadU32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a + b;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<uint32_t> value = {42, 48};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint32_t>(ret), uint32_t(42 + 48));
}

TEST(BinaryOperatorTest, Test4) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a - b;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {409., 123};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<float>(ret), value[0] - value[1]);
}

TEST(BinaryOperatorTest, Test5) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a * b;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {409., 123};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<float>(ret), value[0] * value[1]);
}

TEST(BinaryOperatorTest, Test6) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a / b;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {409., 123};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<float>(ret), value[0] / value[1]);
}

TEST(BinaryOperatorTest, Test7) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kU32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadU32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a % b;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<uint32_t> value = {420, 48};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint32_t>(ret), value[0] % value[1]);
}

TEST(BinaryOperatorTest, Test8) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kU32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadU32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a << b;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<uint32_t> value = {420, 2};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint32_t>(ret), value[0] << value[1]);
}

TEST(BinaryOperatorTest, Test9) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kU32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadU32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a >> b;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<uint32_t> value = {40942982, 2};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint32_t>(ret), value[0] >> value[1]);
}

TEST(BinaryOperatorTest, Test10) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kU32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadU32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = load(entry_arg, 2);
  r = a ^ b ^ c;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<uint32_t> value = {40942982, 2, 12321897};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint32_t>(ret), value[0] ^ value[1] ^ value[2]);
}

TEST(BinaryOperatorTest, Test11) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = 99999 < 10;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(BinaryOperatorTest, Test12) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = 99912739f64 <= 10i64;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(BinaryOperatorTest, Test13) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = 99999f32 == 10u8;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(BinaryOperatorTest, Test14) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = 99999f32 == 99999f64;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(BinaryOperatorTest, Test15) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = 99999f32 >= 10000000i64;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(BinaryOperatorTest, Test16) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = 99999u64 > 109f32;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(BinaryOperatorTest, Test17) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = 99999u64 & 109i32;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint64_t>(ret), uint64_t(99999) & int32_t(109));
}

TEST(BinaryOperatorTest, Test18) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = 99999i64 | 109u32;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<int64_t>(ret), int64_t(99999) | uint32_t(109));
}

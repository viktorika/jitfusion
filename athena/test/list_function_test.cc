#include "gtest/gtest.h"
#include "test_helper.h"

using athena::Athena;
using athena::FunctionRegistry;
using athena::FunctionRegistryFactory;
using athena::FunctionSignature;
using athena::RetType;
using athena::ValueType;
using test::LoadF64List;
using test::LoadI32List;
using test::LoadStringList;

TEST(ListFunctionTest, Test1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32List);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32List)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a + b;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::array<std::vector<int32_t>, 2> value = {std::vector<int32_t>{0, 1, 2, 3, 4}, {0, 1, 4, 9, 16}};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  std::vector<int32_t> expect{0, 1, 2, 3, 4, 0, 1, 4, 9, 16};
  EXPECT_EQ(std::get<std::vector<std::int32_t>>(ret), expect);
}

TEST(ListFunctionTest, Test2) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32List);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32List)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a + b + a + b;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::array<std::vector<int32_t>, 2> value = {std::vector<int32_t>{0, 1, 2, 3, 4}, {0, 1, 4, 9, 16}};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  std::vector<int32_t> expect{0, 1, 2, 3, 4, 0, 1, 4, 9, 16, 0, 1, 2, 3, 4, 0, 1, 4, 9, 16};
  EXPECT_EQ(std::get<std::vector<std::int32_t>>(ret), expect);
}

TEST(ListFunctionTest, Test3) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32List);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32List)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = (a + b + [0, 1, 2, 3, 4, 0, 1, 4, 9, 16]);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::array<std::vector<int32_t>, 2> value = {std::vector<int32_t>{0, 1, 2, 3, 4}, {0, 1, 4, 9, 16}};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  std::vector<int32_t> expect{0, 1, 2, 3, 4, 0, 1, 4, 9, 16, 0, 1, 2, 3, 4, 0, 1, 4, 9, 16};
  EXPECT_EQ(std::get<std::vector<std::int32_t>>(ret), expect);
}

TEST(ListFunctionTest, Test4) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32List);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32List)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = (1 in a and 1 in b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::array<std::vector<int32_t>, 2> value = {std::vector<int32_t>{0, 1, 2, 3, 4}, {0, 1, 4, 9, 16}};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(ListFunctionTest, Test5) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kStringList);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadStringList)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a + b;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  std::vector<std::vector<std::string>> value = {{"abcde", "fghij"}, {"abcde", "fghij"}};
  RetType ret;
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  std::vector<std::string> expect = {"abcde", "fghij", "abcde", "fghij"};
  EXPECT_EQ(std::get<std::vector<std::string>>(ret), expect);
}

TEST(ListFunctionTest, Test6) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kStringList);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadStringList)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = "abcde" in (a + b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  std::vector<std::vector<std::string>> value = {{"abcde", "fghij"}, {"abcde", "fghij"}};
  RetType ret;
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(ListFunctionTest, Test7) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF64List);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF64List)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = 22.22 in (a + b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  std::vector<std::vector<double>> value = {{11.11, 22.22}, {33333.33333, 44444.44444}};
  RetType ret;
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(ListFunctionTest, Test8) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF64List);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF64List)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = Len((a + b) + (a + b) + (b + a) + (b + a));
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  std::vector<std::vector<double>> value = {{11.11, 22.22}, {33333.33333, 44444.44444}};
  RetType ret;
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint32_t>(ret), 16U);
}

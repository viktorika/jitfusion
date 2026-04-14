/*
 * @Author: victorika
 * @Date: 2025-04-09 15:45:31
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-07 15:17:10
 */
#include "athena.h"
#include <array>
#include <cmath>
#include "function_registry.h"
#include "gtest/gtest.h"
#include "type.h"

using athena::Athena;
using jitfusion::ExecContext;
using jitfusion::F64ListStruct;
using jitfusion::FunctionRegistry;
using jitfusion::FunctionRegistryFactory;
using jitfusion::FunctionSignature;
using jitfusion::I32ListStruct;
using jitfusion::RetType;
using jitfusion::Status;
using jitfusion::StringListStruct;
using jitfusion::StringStruct;
using jitfusion::ValueType;

namespace {
uint32_t LoadU32(void* entry_arguments, int32_t index) {
  auto* args = reinterpret_cast<uint32_t*>(entry_arguments);
  return args[index];
}

int32_t LoadI32(void* entry_arguments, int32_t index) {
  auto* args = reinterpret_cast<int32_t*>(entry_arguments);
  return args[index];
}

float LoadF32(void* entry_arguments, int32_t index) {
  auto* args = reinterpret_cast<float*>(entry_arguments);
  return args[index];
}

StringStruct LoadStr(void* entry_arguments, int32_t index) {
  auto* args = reinterpret_cast<std::string*>(entry_arguments);
  StringStruct result;
  result.data = args[index].data();
  result.len = args[index].size();
  return result;
}

StringStruct LoadNullString() {
  StringStruct result;
  result.data = nullptr;
  result.len = 0;
  return result;
}

I32ListStruct LoadI32List(void* entry_arguments, int32_t index) {
  auto* args = reinterpret_cast<std::vector<int32_t>*>(entry_arguments);
  I32ListStruct result;
  result.data = args[index].data();
  result.len = args[index].size();
  return result;
}

F64ListStruct LoadF64List(void* entry_arguments, int32_t index) {
  auto* args = reinterpret_cast<std::vector<double>*>(entry_arguments);
  F64ListStruct result;
  result.data = args[index].data();
  result.len = args[index].size();
  return result;
}

StringListStruct LoadStringList(void* entry_arguments, int32_t index) {
  auto* args = reinterpret_cast<std::vector<std::string>*>(entry_arguments);
  StringListStruct result;
  result.data = new StringStruct[args[index].size()];
  result.len = args[index].size();
  for (size_t i = 0; i < args[index].size(); ++i) {
    result.data[i].data = args[index][i].data();
    result.data[i].len = args[index][i].size();
  }
  return result;
}

void StoreF32(void* output, int32_t index, float value) { reinterpret_cast<float*>(output)[index] = value; }

}  // namespace

TEST(ConstValueTest, Test1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile("r = 42;", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), 42);
}

TEST(ConstValueTest, Test2) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile("r = 42f64;", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), 42., 1e-9);
}

TEST(ConstValueTest, Test3) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile("r = [1, 2, 3];", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  std::vector<int32_t> expected{1, 2, 3};
  std::vector<int32_t> actual = std::get<std::vector<int32_t>>(ret);
  EXPECT_EQ(expected, actual);
}

TEST(ConstValueTest, Test4) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(athena.Compile(R"(r = "abcde";)", func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<std::string>(ret), "abcde");
}

TEST(ConstValueTest, ListMergeOptimizeTest) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  a = [1, 2, 3];
  b = [1, 2, 3];
  c = ListAdd(a, b, exec_ctx);
  d = ListAdd(b, a, exec_ctx);
  e = ListAdd(c, d, exec_ctx);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  std::vector<int32_t> expect{4, 8, 12};
  EXPECT_EQ(std::get<std::vector<std::int32_t>>(ret), expect);
}

TEST(StringAddTest, Test1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ASSERT_TRUE(
      athena
          .Compile(
              R"(r = "*&^$%^&$&^" + "abcdhdaskjdhsakj" + "asdhiuewhiucubwiochgbieuoasgfcuydskjgbcjhkszdgcsdjhakgcasjk";)",
              func_registry)
          .ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<std::string>(ret),
            "*&^$%^&$&^abcdhdaskjdhsakjasdhiuewhiucubwiochgbieuoasgfcuydskjgbcjhkszdgcsdjhakgcasjk");
}

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

TEST(FunctionTest, Test1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = log(12f32);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_NEAR(std::get<float>(ret), std::log(12.0F), 1e-9);
}

TEST(FunctionTest, Test2) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = exp(-8.0);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), std::exp(-8.), 1e-9);
}

TEST(FunctionTest, Test3) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = abs(-8.0);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), std::abs(-8.), 1e-9);
}

TEST(FunctionTest, Test4) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = abs(-144);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), std::abs(-144));
}

TEST(FunctionTest, Test5) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = log2(1444444444.0);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), std::log2(1444444444.), 1e-6);
}

TEST(FunctionTest, Test6) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = log10(1444444444.0);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), std::log10(1444444444.), 1e-6);
}

TEST(FunctionTest, Test7) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = log10(1444444444);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), std::log10(1444444444), 1e-6);
}

TEST(FunctionTest, Test8) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = exp(14);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), std::exp(14), 1e-6);
}

TEST(FunctionTest, Test9) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = sin(0.6);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), std::sin(0.6), 1e-6);
}

TEST(FunctionTest, Test10) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = cos(0.6);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), std::cos(0.6), 1e-6);
}

TEST(FunctionTest, Test11) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = tan(0.6);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), std::tan(0.6), 1e-6);
}

TEST(FunctionTest, Test12) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = sqrt(0.6);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), std::sqrt(0.6), 1e-6);
}

TEST(FunctionTest, Test13) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kU32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadU32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = if(a < b, a, b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<uint32_t> value = {42, 48};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint32_t>(ret), 42U);
}

TEST(FunctionTest, Test14) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = if(round(0.6) == 0.6, floor(0.6), ceil(0.6));
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), 1., 1e-6);
}

TEST(FunctionTest, Test15) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kU32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadU32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = max(a, b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<uint32_t> value = {42, 48};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint32_t>(ret), 48U);
}

TEST(FunctionTest, Test16) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kU32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadU32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = min(a, b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<uint32_t> value = {42, 48};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint32_t>(ret), 42U);
}

TEST(FunctionTest, Test17) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = trunc(6666.6666);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), 6666., 1e-6);
}

TEST(FunctionTest, Test18) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  r = frac(a);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {6666.6666, 123};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_NEAR(std::get<float>(ret), value[0] - std::trunc(value[0]), 1e-6);
}

TEST(FunctionTest, Test19) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  r = frac(a) + trunc(a);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {6666.6666, 123};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_NEAR(std::get<float>(ret), value[0], 1e-6);
}

TEST(FunctionTest, Test20) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = max(a, b) - min(b, a) != abs(a - b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {111.222, 333.444};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(FunctionTest, Test21) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = max(11.22, 33.44) - min(11.22, 33.44) == abs(11.22 - 33.44);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(FunctionTest, Test22) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = sin(a) / cos(a) < sin(b) / cos(b) == tan(a) < tan(b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {0.6, 0.9};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(FunctionTest, Test23) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = abs(a) ^ max(b, 0i32);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<int32_t> value = {-666, 6666};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), std::abs(value[0]) ^ value[1]);
}

TEST(FunctionTest, Test24) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  r = if (if (false, false, false), if (true, true, true), if (false, false, false));
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), false);
}

TEST(FunctionTest, Test25) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = if (a >= b, if (a == b, a + b, a - b), if (a < b, a * b, a / b));
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {999, 555};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<float>(ret), value[0] - value[1]);
}

TEST(FunctionTest, Test26) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = if (a < b, a, b);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {999, 555};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<float>(ret), value[1]);
}

TEST(StringFunctionTest, Test1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kString);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadStr)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  r = a + "wwwwwwwwwwwwwww";
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<std::string> value = {"abcde"};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<std::string>(ret), "abcdewwwwwwwwwwwwwww");
}

TEST(StringFunctionTest, Test2) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kString);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadStr)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  r = (StringLen(a + "www") + 101);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<std::string> value = {"abcde"};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint32_t>(ret), uint32_t(101 + 5 + 3));
}

TEST(StringFunctionTest, Test3) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kString);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadStr)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a > b;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<std::string> value = {"abcde", "bcdef"};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), value[0] > value[1]);
}

TEST(StringFunctionTest, Test4) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kString);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadStr)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = (if(a < b, a, b));
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<std::string> value = {"abcde", "bcdef"};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<std::string>(ret), value[0]);
}

TEST(StringFunctionTest, Test5) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kString);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadStr)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = ((a >= b) == (a <= b));
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<std::string> value = {"abcde", "abcde"};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

TEST(StringFunctionTest, Test6) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kString);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadStr)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  r = (a + a + a + a);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<std::string> value = {"abcde", "abcde"};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<std::string>(ret), "abcdeabcdeabcdeabcde");
}

TEST(StringFunctionTest, Test7) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {}, ValueType::kString);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadNullString)).ok());
  std::string code = R"(
  a = load();
  b = load();
  a == b;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<std::string> value = {};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_EQ(std::get<uint8_t>(ret), true);
}

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

TEST(MixTest, Test1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  std::string code = R"(
  v1 = load(entry_arg, 0);
  r = if(ceil(v1/0.05) > 12, 12, ceil(v1/0.05));
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::vector<float> value = {0.1};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  EXPECT_NEAR(std::get<double>(ret), std::ceil(value[0] / 0.05) > 12 ? 12 : std::ceil(value[0] / 0.05), 1e-6);
}

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

TEST(ComplexTest, Test1) {
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
  s = load(entry_arg, 0);
  s9 = load(entry_arg, 1);
  s21 = load(entry_arg, 2);
  s22 = load(entry_arg, 3);
  r = switch(s21==10 and (s22==1004070 or s22==1003073) and s9==2, s*1.8, s21==10 and (s22==1004070 or s22==1003073) and s9==1,
            s*1.4, (s21==10 and s22==1004116 and s9==1), s*1.5, (s21 != 10 and s9==3), s*1.8, s);
  store(output, 0, CastF32(r));
  )";
  std::string code2 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a - b;
  store(output, 1, r);
  )";
  std::vector<std::string> codes = {code1, code2};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());
  RetType ret;
  std::vector<float> value = {12, 321, 31, 0.9};
  std::vector<float> output = {0, 0, 0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());
  EXPECT_EQ(output[0], 12);
  EXPECT_EQ(output[1], -309);
}

TEST(FilterTest, Test1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32List);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32List)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = GenLargeBitmap(a, 3, exec_ctx);
  FilterByBitmap(a, b, CountBits(b), exec_ctx);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::array<std::vector<int32_t>, 2> value = {std::vector<int32_t>{0, 1, 2, 3, 4, 0, 1, 4, 9, 16}};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  std::vector<int32_t> expect{4, 4, 9, 16};
  EXPECT_EQ(std::get<std::vector<std::int32_t>>(ret), expect);
}

TEST(FilterTest, Test2) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32List);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32List)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = GenLargeBitmap(a, b, exec_ctx);
  FilterByBitmap(a, c, CountBits(c), exec_ctx);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::array<std::vector<int32_t>, 2> value = {std::vector<int32_t>{0, 1, 2, 3, 4, 0, 1, 4, 9, 16},
                                               std::vector<int32_t>{3, 3, 3, 3, 3, 3, 3, 3, 3, 3}};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  std::vector<int32_t> expect{4, 4, 9, 16};
  EXPECT_EQ(std::get<std::vector<std::int32_t>>(ret), expect);
}

TEST(CustomPassTest, CommutativeCallCanonicalizerPass1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32List);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32List)).ok());
  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  c = ListAdd(a, b, exec_ctx);
  d = ListAdd(b, a, exec_ctx);
  e = ListAdd(c, d, exec_ctx);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::array<std::vector<int32_t>, 2> value = {std::vector<int32_t>{1, 2, 3}, std::vector<int32_t>{4, 5, 6}};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  std::vector<int32_t> expect{10, 14, 18};
  EXPECT_EQ(std::get<std::vector<std::int32_t>>(ret), expect);
}

TEST(CustomPassTest, CommutativeCallCanonicalizerPass2) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kI32List);
  EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadI32List)).ok());
  std::string code = R"(
  a = [1, 2, 3];
  b = [4, 5, 6];
  c = ListAdd(a, b, exec_ctx);
  d = ListAdd(b, a, exec_ctx);
  e = ListAdd(c, d, exec_ctx);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::array<std::vector<int32_t>, 2> value = {std::vector<int32_t>{1, 2, 3}, std::vector<int32_t>{4, 5, 6}};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  std::vector<int32_t> expect{10, 14, 18};
  EXPECT_EQ(std::get<std::vector<std::int32_t>>(ret), expect);
}

TEST(PipelineGrouperTest, ThreeGroupTest) {
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
  r = a + b;
  store(output, 0, r);
  )";
  std::string code2 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a * b;
  store(output, 1, r);
  )";

  std::string code3 = R"(
  c = load(entry_arg, 2);
  d = load(entry_arg, 3);
  r = c - d;
  store(output, 2, r);
  )";
  std::string code4 = R"(
  c = load(entry_arg, 2);
  d = load(entry_arg, 3);
  r = c / d;
  store(output, 3, r);
  )";

  std::string code5 = R"(
  e = load(entry_arg, 4);
  f = load(entry_arg, 5);
  r = max(e, f);
  store(output, 4, r);
  )";
  std::string code6 = R"(
  e = load(entry_arg, 4);
  f = load(entry_arg, 5);
  r = min(e, f);
  store(output, 5, r);
  )";

  std::vector<std::string> codes = {code1, code2, code3, code4, code5, code6};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {10, 3, 20, 4, 50, 7};
  std::vector<float> output = {0, 0, 0, 0, 0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 10 + 3);
  EXPECT_EQ(output[1], 10 * 3);
  EXPECT_EQ(output[2], 20 - 4);
  EXPECT_EQ(output[3], 20.0F / 4);
  EXPECT_EQ(output[4], 50);
  EXPECT_EQ(output[5], 7);
}

TEST(ExecErrorTest, SingleExpressionRuntimeError) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  a = [1, 2, 3];
  b = [4, 5];
  r = ListAdd(a, b, exec_ctx);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  Status st = athena.Execute(nullptr, &ret);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("ListAddList"), std::string::npos);
}

TEST(ExecErrorTest, WithExecContextRuntimeError) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  std::string code = R"(
  a = [1, 2, 3];
  b = [4, 5];
  r = ListAdd(a, b, exec_ctx);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  ExecContext exec_ctx(4096);
  RetType ret;
  Status st = athena.Execute(exec_ctx, nullptr, &ret);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("ListAddList"), std::string::npos);
  EXPECT_FALSE(exec_ctx.HasErrors());
}

TEST(ExecErrorTest, BatchExecutePartialFailure) {
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
  r = a + b;
  store(output, 0, r);
  )";
  std::string code2 = R"(
  r = ListAdd([1, 2, 3], [4, 5], exec_ctx);
  )";
  std::string code3 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a * b;
  store(output, 1, r);
  )";

  std::vector<std::string> codes = {code1, code2, code3};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {10, 3};
  std::vector<float> output = {0, 0};
  Status st = athena.Execute(value.data(), output.data());

  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("ListAddList"), std::string::npos);

  EXPECT_EQ(output[0], 10 + 3);
  EXPECT_EQ(output[1], 10 * 3);
}

TEST(ComplexTest, TwoStoreInSingleCode) {
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
  store(output, 0, a + b);
  store(output, 1, a * b);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {5, 3};
  std::vector<float> output = {0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 5 + 3);
  EXPECT_EQ(output[1], 5 * 3);
}

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
  EXPECT_NE(st.ToString().find("Logical operator only supports numeric types"), std::string::npos);
}

TEST(LogicalOpTest, StringOrReject) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  auto st = athena.Compile(R"(r = "hello" or "world";)", func_registry);
  EXPECT_FALSE(st.ok());
  EXPECT_NE(st.ToString().find("Logical operator only supports numeric types"), std::string::npos);
}

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

namespace {

struct CSETestData {
  std::array<float, 8> values;
  int32_t load_count;
  int32_t compute_count;
  int32_t transform_count;
};

float CountedLoadF32(void* entry_arguments, int32_t index) {
  auto* data = reinterpret_cast<CSETestData*>(entry_arguments);
  data->load_count++;
  return data->values[index];
}

float CountedComputeF32(void* entry_arguments, float a, float b) {
  auto* data = reinterpret_cast<CSETestData*>(entry_arguments);
  data->compute_count++;
  return a + b;
}

float CountedTransformF32(void* entry_arguments, float a) {
  auto* data = reinterpret_cast<CSETestData*>(entry_arguments);
  data->transform_count++;
  return a * 2.0f;
}

}  // namespace

TEST(CSETest, SamePipelineRefNodeReuse) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("counted_load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedLoadF32)).ok());
  }
  {
    FunctionSignature sign("counted_compute", {ValueType::kPtr, ValueType::kF32, ValueType::kF32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedComputeF32)).ok());
  }
  {
    FunctionSignature sign("counted_transform", {ValueType::kPtr, ValueType::kF32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedTransformF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }
  std::string code = R"(
  a1 = counted_load(entry_arg, 0);
  a2 = counted_load(entry_arg, 0);
  a3 = counted_load(entry_arg, 0);
  b1 = counted_load(entry_arg, 1);
  b2 = counted_load(entry_arg, 1);
  c1 = counted_compute(entry_arg, a1, b1);
  c2 = counted_compute(entry_arg, a2, b2);
  c3 = counted_compute(entry_arg, c1, c2);
  c4 = counted_compute(entry_arg, c3, c3);
  t1 = counted_transform(entry_arg, c3);
  t2 = counted_transform(entry_arg, c3);
  store(output, 0, a1 + a2 + a3);
  store(output, 1, b1 + b2);
  store(output, 2, c1 + c2);
  store(output, 3, c3);
  store(output, 4, c4);
  store(output, 5, t1 + t2);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  CSETestData data{};
  data.values[0] = 10;
  data.values[1] = 3;
  std::vector<float> output(6, 0);
  ASSERT_TRUE(athena.Execute(&data, output.data()).ok());

  EXPECT_EQ(output[0], 30);
  EXPECT_EQ(output[1], 6);
  EXPECT_EQ(output[2], 26);
  EXPECT_EQ(output[3], 26);
  EXPECT_EQ(output[4], 52);
  EXPECT_EQ(output[5], 104);

  EXPECT_EQ(data.load_count, 2);
  EXPECT_EQ(data.compute_count, 3);
  EXPECT_EQ(data.transform_count, 1);
}

TEST(CSETest, MultiplePipelinesLoadMerge) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("counted_load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedLoadF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code1 = R"(
  a = counted_load(entry_arg, 0);
  b = counted_load(entry_arg, 1);
  c = a + b;
  store(output, 0, c);
  )";

  std::string code2 = R"(
  a = counted_load(entry_arg, 0);
  b = counted_load(entry_arg, 1);
  c = a - b;
  store(output, 1, c);
  )";

  std::vector<std::string> codes = {code1, code2};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  CSETestData data{};
  data.values[0] = 10;
  data.values[1] = 3;
  std::vector<float> output = {0, 0};
  ASSERT_TRUE(athena.Execute(&data, output.data()).ok());

  EXPECT_EQ(output[0], 13);
  EXPECT_EQ(output[1], 7);

  EXPECT_EQ(data.load_count, 2);
}

TEST(CSETest, ThreePipelinesLoadMerge) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("counted_load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedLoadF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code1 = R"(
  a = counted_load(entry_arg, 0);
  store(output, 0, a + 1.0f32);
  )";

  std::string code2 = R"(
  a = counted_load(entry_arg, 0);
  store(output, 1, a + 2.0f32);
  )";

  std::string code3 = R"(
  a = counted_load(entry_arg, 0);
  store(output, 2, a + 3.0f32);
  )";

  std::vector<std::string> codes = {code1, code2, code3};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  CSETestData data{};
  data.values[0] = 10;
  std::vector<float> output = {0, 0, 0};
  ASSERT_TRUE(athena.Execute(&data, output.data()).ok());

  EXPECT_EQ(output[0], 11);
  EXPECT_EQ(output[1], 12);
  EXPECT_EQ(output[2], 13);

  EXPECT_EQ(data.load_count, 1);
}

TEST(CSETest, ReadOnlyComputeFunctionMerge) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("counted_load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedLoadF32)).ok());
  }
  {
    FunctionSignature sign("counted_compute", {ValueType::kPtr, ValueType::kF32, ValueType::kF32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedComputeF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code1 = R"(
  a = counted_load(entry_arg, 0);
  b = counted_load(entry_arg, 1);
  c = counted_compute(entry_arg, a, b);
  store(output, 0, c);
  )";

  std::string code2 = R"(
  a = counted_load(entry_arg, 0);
  b = counted_load(entry_arg, 1);
  c = counted_compute(entry_arg, a, b);
  store(output, 1, c + 1.0f32);
  )";

  std::vector<std::string> codes = {code1, code2};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  CSETestData data{};
  data.values[0] = 5;
  data.values[1] = 3;
  std::vector<float> output = {0, 0};
  ASSERT_TRUE(athena.Execute(&data, output.data()).ok());

  EXPECT_EQ(output[0], 8);
  EXPECT_EQ(output[1], 9);

  EXPECT_EQ(data.load_count, 2);
  EXPECT_EQ(data.compute_count, 1);
}

TEST(CSETest, DifferentArgsNoMerge) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("counted_load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedLoadF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code1 = R"(
  a = counted_load(entry_arg, 0);
  store(output, 0, a);
  )";

  std::string code2 = R"(
  b = counted_load(entry_arg, 1);
  store(output, 1, b);
  )";

  std::vector<std::string> codes = {code1, code2};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  CSETestData data{};
  data.values[0] = 10;
  data.values[1] = 20;
  std::vector<float> output = {0, 0};
  ASSERT_TRUE(athena.Execute(&data, output.data()).ok());

  EXPECT_EQ(output[0], 10);
  EXPECT_EQ(output[1], 20);

  EXPECT_EQ(data.load_count, 2);
}

TEST(CSETest, ChainedReadOnlyFunctionMerge) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("counted_load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedLoadF32)).ok());
  }
  {
    FunctionSignature sign("counted_transform", {ValueType::kPtr, ValueType::kF32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedTransformF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code1 = R"(
  a = counted_load(entry_arg, 0);
  b = counted_transform(entry_arg, a);
  store(output, 0, b);
  )";

  std::string code2 = R"(
  a = counted_load(entry_arg, 0);
  b = counted_transform(entry_arg, a);
  store(output, 1, b + 1.0f32);
  )";

  std::vector<std::string> codes = {code1, code2};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  CSETestData data{};
  data.values[0] = 5;
  std::vector<float> output = {0, 0};
  ASSERT_TRUE(athena.Execute(&data, output.data()).ok());

  EXPECT_EQ(output[0], 10);
  EXPECT_EQ(output[1], 11);

  EXPECT_EQ(data.load_count, 1);
  EXPECT_EQ(data.transform_count, 1);
}

TEST(CSETest, PartialMerge) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("counted_load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedLoadF32)).ok());
  }
  {
    FunctionSignature sign("counted_compute", {ValueType::kPtr, ValueType::kF32, ValueType::kF32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedComputeF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code1 = R"(
  a = counted_load(entry_arg, 0);
  b = counted_load(entry_arg, 1);
  r = counted_compute(entry_arg, a, b);
  store(output, 0, r);
  )";

  std::string code2 = R"(
  a = counted_load(entry_arg, 0);
  b = counted_load(entry_arg, 1);
  r = counted_compute(entry_arg, a, b);
  store(output, 1, r + 10.0f32);
  )";

  std::string code3 = R"(
  a = counted_load(entry_arg, 0);
  c = counted_load(entry_arg, 2);
  r = counted_compute(entry_arg, a, c);
  store(output, 2, r);
  )";

  std::vector<std::string> codes = {code1, code2, code3};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  CSETestData data{};
  data.values[0] = 5;
  data.values[1] = 3;
  data.values[2] = 7;
  std::vector<float> output = {0, 0, 0};
  ASSERT_TRUE(athena.Execute(&data, output.data()).ok());

  EXPECT_EQ(output[0], 8);
  EXPECT_EQ(output[1], 18);
  EXPECT_EQ(output[2], 12);

  EXPECT_EQ(data.load_count, 3);
  EXPECT_EQ(data.compute_count, 2);
}

TEST(CSETest, ComplexPipelineCSE) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("counted_load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedLoadF32)).ok());
  }
  {
    FunctionSignature sign("counted_compute", {ValueType::kPtr, ValueType::kF32, ValueType::kF32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedComputeF32)).ok());
  }
  {
    FunctionSignature sign("counted_transform", {ValueType::kPtr, ValueType::kF32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedTransformF32)).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kVoid);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code1 = R"(
  a = counted_load(entry_arg, 0);
  b = counted_load(entry_arg, 1);
  c = counted_compute(entry_arg, a, b);
  d = counted_transform(entry_arg, c);
  store(output, 0, d);
  )";

  std::string code2 = R"(
  a = counted_load(entry_arg, 0);
  b = counted_load(entry_arg, 1);
  c = counted_compute(entry_arg, a, b);
  d = counted_transform(entry_arg, c);
  store(output, 1, d + 100.0f32);
  )";

  std::string code3 = R"(
  a = counted_load(entry_arg, 0);
  b = counted_load(entry_arg, 1);
  c = counted_compute(entry_arg, a, b);
  store(output, 2, c);
  )";

  std::vector<std::string> codes = {code1, code2, code3};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  CSETestData data{};
  data.values[0] = 5;
  data.values[1] = 3;
  std::vector<float> output = {0, 0, 0};
  ASSERT_TRUE(athena.Execute(&data, output.data()).ok());

  EXPECT_EQ(output[0], 16);
  EXPECT_EQ(output[1], 116);
  EXPECT_EQ(output[2], 8);

  EXPECT_EQ(data.load_count, 2);
  EXPECT_EQ(data.compute_count, 1);
  EXPECT_EQ(data.transform_count, 1);
}

GTEST_API_ int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
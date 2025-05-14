/*
 * @Author: victorika
 * @Date: 2025-04-09 15:45:31
 * @Last Modified by: victorika
 * @Last Modified time: 2025-04-16 16:52:47
 */
#include "athena.h"
#include <array>
#include <cmath>
#include "function_registry.h"
#include "gtest/gtest.h"
#include "type.h"

using athena::Athena;
using jitfusion::F64ListStruct;
using jitfusion::FunctionRegistry;
using jitfusion::FunctionRegistryFactory;
using jitfusion::FunctionSignature;
using jitfusion::FunctionStructure;
using jitfusion::FunctionType;
using jitfusion::I32ListStruct;
using jitfusion::RetType;
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

int32_t StoreF32(void* output, int32_t index, float value) {
  reinterpret_cast<float*>(output)[index] = value;
  return 0;
}

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

TEST(ComplexTest, Test1) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(LoadF32), 1).ok());
  }
  {
    FunctionSignature sign("store", {ValueType::kPtr, ValueType::kI32, ValueType::kF32}, ValueType::kI32);
    EXPECT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreF32), 1).ok());
  }

  std::string code1 = R"(
  s = load(entry_arg, 0);
  s9 = load(entry_arg, 1);
  s21 = load(entry_arg, 2);
  s22 = load(entry_arg, 3);
  r = switch(s21==10 and (s22==1004070 or s22==1003073) and s9==2, s*1.8, s21==10 and (s22==1004070 or s22==1003073) and s9==1,
            s*1.4, (s21==10 and s22==1004116 and s9==1), s*1.5, (s21 != 10 and s9==3), s*1.8, s);
  s_ret = store(output, 0, CastF32(r));
  )";
  std::string code2 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a - b;
  s_ret = store(output, 1, r);
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
  r = FilterByBitmap(a, b, CountBits(b), exec_ctx);
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());
  RetType ret;
  std::array<std::vector<int32_t>, 2> value = {std::vector<int32_t>{0, 1, 2, 3, 4, 0, 1, 4, 9, 16}};
  ASSERT_TRUE(athena.Execute(value.data(), &ret).ok());
  std::vector<int32_t> expect{4, 4, 9, 16};
  EXPECT_EQ(std::get<std::vector<std::int32_t>>(ret), expect);
}

GTEST_API_ int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
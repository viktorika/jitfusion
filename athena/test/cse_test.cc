#include "gtest/gtest.h"
#include "test_helper.h"

using athena::Athena;
using athena::FunctionRegistry;
using athena::FunctionRegistryFactory;
using athena::FunctionSignature;
using athena::RetType;
using athena::ValueType;
using test::StoreF32;

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

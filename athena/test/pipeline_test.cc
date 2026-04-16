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

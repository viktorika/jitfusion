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

TEST(IsolatedScopeTest, SameVarNameDifferentComputation) {
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
  result = (a + b) * 2.0f32;
  store(output, 0, result);
  )";

  std::string code2 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  result = (a - b) * 3.0f32;
  store(output, 1, result);
  )";

  std::vector<std::string> codes = {code1, code2};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {10, 3};
  std::vector<float> output = {0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 26);
  EXPECT_EQ(output[1], 21);
}

TEST(IsolatedScopeTest, SameIntermediateVarDifferentChain) {
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
  x = load(entry_arg, 0);
  y = load(entry_arg, 1);
  tmp = x + y;
  result = tmp * tmp;
  store(output, 0, result);
  )";

  std::string code2 = R"(
  x = load(entry_arg, 0);
  y = load(entry_arg, 1);
  tmp = x * y;
  result = tmp + 1.0f32;
  store(output, 1, result);
  )";

  std::string code3 = R"(
  x = load(entry_arg, 0);
  y = load(entry_arg, 1);
  tmp = x / y;
  result = tmp - 1.0f32;
  store(output, 2, result);
  )";

  std::vector<std::string> codes = {code1, code2, code3};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {6, 2};
  std::vector<float> output = {0, 0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 64);
  EXPECT_EQ(output[1], 13);
  EXPECT_EQ(output[2], 2);
}

TEST(IsolatedScopeTest, CrossPipelineVarReferenceError) {
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
  my_var = a * 2.0f32;
  store(output, 0, my_var);
  )";

  std::string code2 = R"(
  b = load(entry_arg, 1);
  result = b + my_var;
  store(output, 1, result);
  )";

  std::vector<std::string> codes = {code1, code2};
  auto status = athena.Compile(codes, func_registry);
  EXPECT_FALSE(status.ok());
}

TEST(IsolatedScopeTest, SameVarNameDifferentLoadIndex) {
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
  val = load(entry_arg, 0);
  r = val * 10.0f32;
  store(output, 0, r);
  )";

  std::string code2 = R"(
  val = load(entry_arg, 1);
  r = val * 20.0f32;
  store(output, 1, r);
  )";

  std::string code3 = R"(
  val = load(entry_arg, 2);
  r = val * 30.0f32;
  store(output, 2, r);
  )";

  std::vector<std::string> codes = {code1, code2, code3};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {1, 2, 3};
  std::vector<float> output = {0, 0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 10);
  EXPECT_EQ(output[1], 40);
  EXPECT_EQ(output[2], 90);
}

TEST(IsolatedScopeTest, SameVarNameWithWhenBlock) {
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
  r = 0.0f32;
  when a > 5.0f32 {
    r = a * 2.0f32;
  }
  store(output, 0, r);
  )";

  std::string code2 = R"(
  a = load(entry_arg, 0);
  r = 100.0f32;
  when a > 5.0f32 {
    r = a * 3.0f32;
  }
  store(output, 1, r);
  )";

  std::vector<std::string> codes = {code1, code2};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {10};
  std::vector<float> output = {0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 20);
  EXPECT_EQ(output[1], 30);
}

TEST(IsolatedScopeTest, SameVarNameWithWhenElifElse) {
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
  r = 0.0f32;
  when a > 10.0f32 {
    r = a * 3.0f32;
  } elif a > 5.0f32 {
    r = a * 2.0f32;
  } else {
    r = a;
  }
  store(output, 0, r);
  )";

  std::string code2 = R"(
  a = load(entry_arg, 0);
  r = 0.0f32;
  when a > 10.0f32 {
    r = a + 100.0f32;
  } elif a > 5.0f32 {
    r = a + 50.0f32;
  } else {
    r = a + 10.0f32;
  }
  store(output, 1, r);
  )";

  std::vector<std::string> codes = {code1, code2};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {7};
  std::vector<float> output = {0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 14);
  EXPECT_EQ(output[1], 57);
}

TEST(IsolatedScopeTest, FourPipelinesSameVarNames) {
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
  x = load(entry_arg, 0);
  y = load(entry_arg, 1);
  z = x + y;
  store(output, 0, z);
  )";

  std::string code2 = R"(
  x = load(entry_arg, 0);
  y = load(entry_arg, 1);
  z = x - y;
  store(output, 1, z);
  )";

  std::string code3 = R"(
  x = load(entry_arg, 0);
  y = load(entry_arg, 1);
  z = x * y;
  store(output, 2, z);
  )";

  std::string code4 = R"(
  x = load(entry_arg, 0);
  y = load(entry_arg, 1);
  z = x / y;
  store(output, 3, z);
  )";

  std::vector<std::string> codes = {code1, code2, code3, code4};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {12, 4};
  std::vector<float> output = {0, 0, 0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 16);
  EXPECT_EQ(output[1], 8);
  EXPECT_EQ(output[2], 48);
  EXPECT_EQ(output[3], 3);
}

TEST(IsolatedScopeTest, DifferentGroupsSameVarNames) {
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
  tmp = a + b;
  store(output, 0, tmp);
  )";

  std::string code2 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  tmp = a * b;
  store(output, 1, tmp);
  )";

  std::string code3 = R"(
  a = load(entry_arg, 2);
  b = load(entry_arg, 3);
  tmp = a - b;
  store(output, 2, tmp);
  )";

  std::string code4 = R"(
  a = load(entry_arg, 2);
  b = load(entry_arg, 3);
  tmp = max(a, b);
  store(output, 3, tmp);
  )";

  std::vector<std::string> codes = {code1, code2, code3, code4};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {5, 3, 20, 7};
  std::vector<float> output = {0, 0, 0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 8);
  EXPECT_EQ(output[1], 15);
  EXPECT_EQ(output[2], 13);
  EXPECT_EQ(output[3], 20);
}

TEST(IsolatedScopeTest, SameVarNameWithNestedWhen) {
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
  r = 0.0f32;
  when a > 0.0f32 {
    r = 1.0f32;
    when a > 10.0f32 {
      r = 2.0f32;
    }
  }
  store(output, 0, r);
  )";

  std::string code2 = R"(
  a = load(entry_arg, 0);
  r = 100.0f32;
  when a > 0.0f32 {
    r = 200.0f32;
    when a > 10.0f32 {
      r = 300.0f32;
    }
  }
  store(output, 1, r);
  )";

  std::vector<std::string> codes = {code1, code2};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {15};
  std::vector<float> output = {0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 2);
  EXPECT_EQ(output[1], 300);
}

TEST(IsolatedScopeTest, SameVarNameStoreInsideWhen) {
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
  val = load(entry_arg, 0);
  when val > 5.0f32 {
    store(output, 0, val * 2.0f32);
  } else {
    store(output, 0, val);
  }
  )";

  std::string code2 = R"(
  val = load(entry_arg, 0);
  when val > 5.0f32 {
    store(output, 1, val + 100.0f32);
  } else {
    store(output, 1, val + 10.0f32);
  }
  )";

  std::vector<std::string> codes = {code1, code2};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  std::vector<float> value = {8};
  std::vector<float> output = {0, 0};
  ASSERT_TRUE(athena.Execute(value.data(), output.data()).ok());

  EXPECT_EQ(output[0], 16);
  EXPECT_EQ(output[1], 108);
}

TEST(ExecContextTest, ExpressionModeWithExecContext) {
  Athena athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kF32);
    EXPECT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(LoadF32)).ok());
  }

  std::string code = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a + b;
  )";
  ASSERT_TRUE(athena.Compile(code, func_registry).ok());

  athena::ExecContext exec_ctx(4096);
  RetType ret;
  std::vector<float> value = {10, 20};
  ASSERT_TRUE(athena.Execute(exec_ctx, value.data(), &ret).ok());
  EXPECT_EQ(std::get<float>(ret), 30);

  std::vector<float> value2 = {5, 3};
  ASSERT_TRUE(athena.Execute(exec_ctx, value2.data(), &ret).ok());
  EXPECT_EQ(std::get<float>(ret), 8);
}

TEST(ExecContextTest, PipelineModeWithExecContext) {
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

  std::vector<std::string> codes = {code1, code2};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  athena::ExecContext exec_ctx(4096);
  std::vector<float> value = {5, 3};
  std::vector<float> output = {0, 0};
  ASSERT_TRUE(athena.Execute(exec_ctx, value.data(), output.data()).ok());
  EXPECT_EQ(output[0], 8);
  EXPECT_EQ(output[1], 15);

  std::vector<float> value2 = {10, 4};
  std::vector<float> output2 = {0, 0};
  ASSERT_TRUE(athena.Execute(exec_ctx, value2.data(), output2.data()).ok());
  EXPECT_EQ(output2[0], 14);
  EXPECT_EQ(output2[1], 40);
}

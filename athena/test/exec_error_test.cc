#include <athena.h>
#include "gtest/gtest.h"
#include "test_helper.h"

using athena::Athena;
using athena::ExecContext;
using athena::FunctionRegistry;
using athena::FunctionRegistryFactory;
using athena::FunctionSignature;
using athena::RetType;
using athena::Status;
using athena::ValueType;
using test::LoadF32;
using test::StoreF32;

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

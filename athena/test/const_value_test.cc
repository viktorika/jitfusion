#include <athena.h>
#include "gtest/gtest.h"
#include "test_helper.h"

using athena::Athena;
using athena::FunctionRegistry;
using athena::FunctionRegistryFactory;
using athena::RetType;

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

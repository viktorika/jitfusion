#include <athena.h>
#include "gtest/gtest.h"
#include "test_helper.h"

using athena::AthenaExpression;
using athena::FunctionRegistry;
using athena::FunctionRegistryFactory;
using athena::RetType;

namespace {

std::unique_ptr<FunctionRegistry> MakeRegistry() {
  std::unique_ptr<FunctionRegistry> reg;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&reg).ok());
  return reg;
}

}  // namespace

// 单行注释 // 在表达式末尾
TEST(CommentTest, LineCommentAtEol) {
  AthenaExpression athena;
  auto reg = MakeRegistry();
  ASSERT_TRUE(athena.Compile("r = 42; // this is a trailing comment\n", reg).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), 42);
}

// 单行注释独占一行
TEST(CommentTest, LineCommentWholeLine) {
  AthenaExpression athena;
  auto reg = MakeRegistry();
  std::string code = R"(
    // top-level comment
    r = 1 + 2;
    // another comment after
  )";
  ASSERT_TRUE(athena.Compile(code, reg).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), 3);
}

// 块注释在表达式中间
TEST(CommentTest, BlockCommentInline) {
  AthenaExpression athena;
  auto reg = MakeRegistry();
  ASSERT_TRUE(athena.Compile("r = 1 /* mid */ + 2;", reg).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), 3);
}

// 跨行块注释
TEST(CommentTest, BlockCommentMultiline) {
  AthenaExpression athena;
  auto reg = MakeRegistry();
  std::string code = R"(
    /* this is
       a multi line
       comment */
    r = 10 * 2;
  )";
  ASSERT_TRUE(athena.Compile(code, reg).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), 20);
}

// 块注释中包含 *、/ 不会提前结束
TEST(CommentTest, BlockCommentWithStars) {
  AthenaExpression athena;
  auto reg = MakeRegistry();
  std::string code = R"(
    /***** banner *****/
    /* a / b * c ** d */
    r = 7;
  )";
  ASSERT_TRUE(athena.Compile(code, reg).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), 7);
}

// 行注释里出现 /* 不会启动块注释
TEST(CommentTest, LineCommentSwallowsBlockOpen) {
  AthenaExpression athena;
  auto reg = MakeRegistry();
  std::string code = R"(
    // this looks like /* but is just a line comment
    r = 5;
  )";
  ASSERT_TRUE(athena.Compile(code, reg).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), 5);
}

// 字符串字面量里的 //、/* 不应被当成注释
TEST(CommentTest, StringLiteralIsNotComment) {
  AthenaExpression athena;
  auto reg = MakeRegistry();
  ASSERT_TRUE(athena.Compile(R"(r = "a // not comment /* still */ b";)", reg).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<std::string>(ret), "a // not comment /* still */ b");
}

// 多语句、混合注释
TEST(CommentTest, MixedCommentsBetweenStatements) {
  AthenaExpression athena;
  auto reg = MakeRegistry();
  std::string code = R"(
    a = 1;            // line
    /* block */ b = 2;
    c = a + b /* trailing */ ;
    r = c;
  )";
  ASSERT_TRUE(athena.Compile(code, reg).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), 3);
}

// 空块注释 /**/
TEST(CommentTest, EmptyBlockComment) {
  AthenaExpression athena;
  auto reg = MakeRegistry();
  ASSERT_TRUE(athena.Compile("r = 1 /**/ + 2;", reg).ok());
  RetType ret;
  ASSERT_TRUE(athena.Execute(nullptr, &ret).ok());
  EXPECT_EQ(std::get<int32_t>(ret), 3);
}

// 仅含注释的代码应当报错（语法上仍需要一个赋值结果）
TEST(CommentTest, OnlyCommentFails) {
  AthenaExpression athena;
  auto reg = MakeRegistry();
  std::string code = R"(
    // just a comment
    /* nothing else */
  )";
  EXPECT_FALSE(athena.Compile(code, reg).ok());
}

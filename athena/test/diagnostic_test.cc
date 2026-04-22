/*
 * @Author: victorika
 * @Date: 2026-04-21 10:45:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-21 10:45:00
 */
#include "diagnostic.h"
#include "athena.h"
#include "function_registry.h"
#include "gtest/gtest.h"

using athena::Athena;
using jitfusion::Diagnostic;
using jitfusion::FunctionRegistry;
using jitfusion::FunctionRegistryFactory;
using jitfusion::SourceLocation;

namespace {

std::unique_ptr<FunctionRegistry> MakeRegistry() {
  std::unique_ptr<FunctionRegistry> reg;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&reg).ok());
  return reg;
}

}  // namespace

TEST(DiagnosticTest, RenderWithValidLocation) {
  std::string source = R"(a = 1;
b = a + "hi";
c = b;
)";
  SourceLocation loc{source, 2, 5, 2, 13};
  Diagnostic diag(Diagnostic::Level::kError, "binary operator type mismatch", loc);
  diag.WithNote("left: i32").WithNote("right: string").WithHint("cast one side explicitly");
  std::string rendered = diag.Render();

  EXPECT_NE(rendered.find("error: binary operator type mismatch"), std::string::npos);
  EXPECT_NE(rendered.find("line 2, column 5"), std::string::npos);
  EXPECT_NE(rendered.find("b = a + \"hi\";"), std::string::npos);  // the offending source line
  EXPECT_NE(rendered.find('^'), std::string::npos);                // a caret underline is drawn
  EXPECT_NE(rendered.find("note: left: i32"), std::string::npos);
  EXPECT_NE(rendered.find("note: right: string"), std::string::npos);
  EXPECT_NE(rendered.find("hint: cast one side explicitly"), std::string::npos);
}

TEST(DiagnosticTest, RenderWithoutLocationFallsBack) {
  Diagnostic diag(Diagnostic::Level::kError, "something went wrong", SourceLocation{});
  diag.WithNote("context info");
  std::string rendered = diag.Render();

  EXPECT_NE(rendered.find("error: something went wrong"), std::string::npos);
  EXPECT_NE(rendered.find("note: context info"), std::string::npos);
  EXPECT_EQ(rendered.find("-->"), std::string::npos);
}

TEST(DiagnosticTest, RenderWithEmptySourceFallsBack) {
  Diagnostic diag(Diagnostic::Level::kError, "boom", SourceLocation{{}, 1, 1, 1, 2});
  std::string rendered = diag.Render();
  EXPECT_NE(rendered.find("error: boom"), std::string::npos);
  EXPECT_EQ(rendered.find("-->"), std::string::npos);
}

TEST(DiagnosticTest, UndefinedVariableProducesSourceContext) {
  Athena athena;
  auto reg = MakeRegistry();
  std::string code = "r = a + 1;";  // 'a' is undefined
  auto st = athena.Compile(code, reg);
  ASSERT_FALSE(st.ok());
  const std::string msg = st.ToString();
  EXPECT_NE(msg.find("variable 'a' is not defined"), std::string::npos) << msg;
  EXPECT_NE(msg.find("line 1"), std::string::npos) << msg;
  EXPECT_NE(msg.find("r = a + 1;"), std::string::npos) << msg;
}

TEST(DiagnosticTest, IfBranchTypeMismatchProducesSourceContext) {
  Athena athena;
  auto reg = MakeRegistry();
  std::string code = "r = if(1, 2, \"hi\");";
  auto st = athena.Compile(code, reg);
  ASSERT_FALSE(st.ok());
  const std::string msg = st.ToString();
  EXPECT_NE(msg.find("if branches have incompatible types"), std::string::npos) << msg;
  EXPECT_NE(msg.find("i32"), std::string::npos) << msg;
  EXPECT_NE(msg.find("string"), std::string::npos) << msg;
  EXPECT_NE(msg.find("line 1"), std::string::npos) << msg;
}

TEST(DiagnosticTest, DivByZeroProducesSourceContext) {
  Athena athena;
  auto reg = MakeRegistry();
  std::string code = "r = 10 / 0;";
  auto st = athena.Compile(code, reg);
  ASSERT_FALSE(st.ok());
  const std::string msg = st.ToString();
  EXPECT_NE(msg.find("division or modulo by constant zero"), std::string::npos) << msg;
}

TEST(DiagnosticTest, DemoRenderedOutput) {
  auto reg = MakeRegistry();
  {
    Athena athena;
    std::string code = R"(a = 10;
b = a + 1;
c = d + 5;
)";
    auto st = athena.Compile(code, reg);
    ASSERT_FALSE(st.ok());
    GTEST_LOG_(INFO) << "\n--- undefined variable ---\n" << st.ToString();
    EXPECT_NE(st.ToString().find("variable 'd' is not defined"), std::string::npos);
    EXPECT_NE(st.ToString().find("line 3"), std::string::npos);
  }
  {
    Athena athena;
    auto st = athena.Compile("r = if(1, 2, \"hi\");", reg);
    ASSERT_FALSE(st.ok());
    GTEST_LOG_(INFO) << "\n--- if branches mismatch ---\n" << st.ToString();
  }
  {
    Athena athena;
    auto st = athena.Compile("r = 10 / 0;", reg);
    ASSERT_FALSE(st.ok());
    GTEST_LOG_(INFO) << "\n--- div by zero ---\n" << st.ToString();
  }
  {
    Athena athena;
    auto st = athena.Compile(R"(r = "hi" and "world";)", reg);
    ASSERT_FALSE(st.ok());
    GTEST_LOG_(INFO) << "\n--- logical op on strings ---\n" << st.ToString();
  }
}

TEST(DiagnosticTest, PipelineUndefinedVariableProducesSourceContext) {
  Athena athena;
  auto reg = MakeRegistry();
  std::string code = R"(a = 10;
b = a + 1;
c = d + 5;
)";
  std::vector<std::string> codes = {code};
  auto st = athena.Compile(codes, reg);
  ASSERT_FALSE(st.ok());
  const std::string msg = st.ToString();
  EXPECT_NE(msg.find("variable 'd' is not defined"), std::string::npos) << msg;
  // 'd' is on line 3, starting at column 5 (after "c = " which is 4 chars).
  EXPECT_NE(msg.find("line 3"), std::string::npos) << msg;
  EXPECT_NE(msg.find("column 5"), std::string::npos) << msg;
  EXPECT_NE(msg.find("c = d + 5;"), std::string::npos) << msg;
}

TEST(DiagnosticTest, MultiPipelineErrorAnchorsToOwningPipeline) {
  Athena athena;
  auto reg = MakeRegistry();
  std::string ok_code = R"(x = 1;
y = x + 2;
)";
  std::string bad_code = R"(p = 100;
q = p + zzz;
)";
  std::vector<std::string> codes = {ok_code, bad_code};
  auto st = athena.Compile(codes, reg);
  ASSERT_FALSE(st.ok());
  const std::string msg = st.ToString();

  EXPECT_NE(msg.find("variable 'zzz' is not defined"), std::string::npos) << msg;
  EXPECT_NE(msg.find("line 2"), std::string::npos) << msg;
  EXPECT_NE(msg.find("column 9"), std::string::npos) << msg;
  EXPECT_NE(msg.find("q = p + zzz;"), std::string::npos) << msg;
  EXPECT_EQ(msg.find("y = x + 2;"), std::string::npos)
      << "pipeline #1 source should not be rendered for an error in pipeline #2: " << msg;
}

TEST(DiagnosticTest, MultiPipelineCompileAndExecuteHappyPath) {
  Athena athena;
  auto reg = MakeRegistry();
  {
    jitfusion::FunctionSignature sign("load", {jitfusion::ValueType::kPtr, jitfusion::ValueType::kI32},
                                      jitfusion::ValueType::kF32);
    ASSERT_TRUE(reg->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(+[](void* arg, int32_t i) -> float {
                                             return static_cast<float*>(arg)[i];
                                           }))
                    .ok());
  }
  {
    jitfusion::FunctionSignature sign(
        "store", {jitfusion::ValueType::kPtr, jitfusion::ValueType::kI32, jitfusion::ValueType::kF32},
        jitfusion::ValueType::kVoid);
    ASSERT_TRUE(reg->RegisterStoreCFunc(sign, reinterpret_cast<void*>(+[](void* out, int32_t i, float v) {
                                          static_cast<float*>(out)[i] = v;
                                        }),
                                        1)
                    .ok());
  }

  std::string code1 = R"(
  a = load(entry_arg, 0);
  b = a + 1.0f32;
  store(output, 0, b);
  )";
  std::string code2 = R"(
  x = load(entry_arg, 1);
  y = x * 2.0f32;
  store(output, 1, y);
  )";
  std::vector<std::string> codes = {code1, code2};
  auto st = athena.Compile(codes, reg);
  ASSERT_TRUE(st.ok()) << st.ToString();

  std::vector<float> input = {10.0F, 3.0F};
  std::vector<float> output = {0.0F, 0.0F};
  ASSERT_TRUE(athena.Execute(input.data(), output.data()).ok());
  EXPECT_FLOAT_EQ(output[0], 11.0F);
  EXPECT_FLOAT_EQ(output[1], 6.0F);
}

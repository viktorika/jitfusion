
#include "pipeline_grouper.h"
#include "ast_builder.h"
#include "gtest/gtest.h"
#include "test_helper.h"

using athena::FunctionRegistry;
using athena::FunctionRegistryFactory;
using athena::FunctionSignature;
using athena::PipelineGrouper;
using athena::ProgramAstBuilder;
using athena::ValueType;
using jitfusion::ExecNode;
using jitfusion::NoOPNode;

namespace {

// Helper: build a pipeline AST from code string
std::unique_ptr<ExecNode> BuildPipeline(const std::string& code) {
  ProgramAstBuilder builder;
  std::unique_ptr<ExecNode> result;
  auto status = builder.BuildPipeline(code, &result);
  EXPECT_TRUE(status.ok());
  return result;
}

// Helper: get the number of children in a grouped NoOPNode
size_t GetChildCount(const std::unique_ptr<ExecNode>& node) {
  auto* noop = dynamic_cast<NoOPNode*>(node.get());
  EXPECT_NE(noop, nullptr);
  return noop->GetArgs().size();
}

// Helper: check if a grouped NoOPNode has isolated flag set
bool IsIsolated(const std::unique_ptr<ExecNode>& node) {
  auto* noop = dynamic_cast<NoOPNode*>(node.get());
  EXPECT_NE(noop, nullptr);
  return noop->IsIsolated();
}

}  // namespace

// Two pipelines loading from the same indices should be merged into 1 group.
TEST(PipelineGrouperGroupTest, SameLoadMergedIntoOneGroup) {
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

  std::vector<std::unique_ptr<ExecNode>> pipelines;
  pipelines.emplace_back(BuildPipeline(code1));
  pipelines.emplace_back(BuildPipeline(code2));

  PipelineGrouper grouper;
  auto groups = grouper.Group(pipelines);

  ASSERT_EQ(groups.size(), 1);
  EXPECT_EQ(GetChildCount(groups[0]), 2);
  EXPECT_TRUE(IsIsolated(groups[0]));
}

// Two pipelines loading from different indices should be separated into 2 groups.
TEST(PipelineGrouperGroupTest, DifferentLoadSeparatedIntoTwoGroups) {
  std::string code1 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a + b;
  store(output, 0, r);
  )";
  std::string code2 = R"(
  a = load(entry_arg, 2);
  b = load(entry_arg, 3);
  r = a + b;
  store(output, 1, r);
  )";

  std::vector<std::unique_ptr<ExecNode>> pipelines;
  pipelines.emplace_back(BuildPipeline(code1));
  pipelines.emplace_back(BuildPipeline(code2));

  PipelineGrouper grouper;
  auto groups = grouper.Group(pipelines);

  ASSERT_EQ(groups.size(), 2);
  EXPECT_EQ(GetChildCount(groups[0]), 1);
  EXPECT_EQ(GetChildCount(groups[1]), 1);
  EXPECT_TRUE(IsIsolated(groups[0]));
  EXPECT_TRUE(IsIsolated(groups[1]));
}

// Three groups: (code1, code2), (code3, code4), (code5, code6)
// Each pair shares the same load indices.
TEST(PipelineGrouperGroupTest, SixPipelinesThreeGroups) {
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

  std::vector<std::unique_ptr<ExecNode>> pipelines;
  pipelines.emplace_back(BuildPipeline(code1));
  pipelines.emplace_back(BuildPipeline(code2));
  pipelines.emplace_back(BuildPipeline(code3));
  pipelines.emplace_back(BuildPipeline(code4));
  pipelines.emplace_back(BuildPipeline(code5));
  pipelines.emplace_back(BuildPipeline(code6));

  PipelineGrouper grouper;
  auto groups = grouper.Group(pipelines);

  ASSERT_EQ(groups.size(), 3);
  // Each group should contain exactly 2 pipelines
  size_t total_children = 0;
  for (const auto& group : groups) {
    EXPECT_TRUE(IsIsolated(group));
    EXPECT_EQ(GetChildCount(group), 2);
    total_children += GetChildCount(group);
  }
  EXPECT_EQ(total_children, 6);
}

// Single pipeline should produce 1 group with 1 child.
TEST(PipelineGrouperGroupTest, SinglePipelineOneGroup) {
  std::string code = R"(
  a = load(entry_arg, 0);
  r = a * 2.0f32;
  store(output, 0, r);
  )";

  std::vector<std::unique_ptr<ExecNode>> pipelines;
  pipelines.emplace_back(BuildPipeline(code));

  PipelineGrouper grouper;
  auto groups = grouper.Group(pipelines);

  ASSERT_EQ(groups.size(), 1);
  EXPECT_EQ(GetChildCount(groups[0]), 1);
  EXPECT_TRUE(IsIsolated(groups[0]));
}

// Empty input should produce empty output.
TEST(PipelineGrouperGroupTest, EmptyInputEmptyOutput) {
  std::vector<std::unique_ptr<ExecNode>> pipelines;

  PipelineGrouper grouper;
  auto groups = grouper.Group(pipelines);

  ASSERT_EQ(groups.size(), 0);
}

// Four pipelines: code1 & code2 share load(0,1), code3 & code4 share load(2,3).
// Should produce 2 groups, each with 2 children.
TEST(PipelineGrouperGroupTest, FourPipelinesTwoGroups) {
  std::string code1 = R"(
  x = load(entry_arg, 0);
  y = load(entry_arg, 1);
  r = x + y;
  store(output, 0, r);
  )";
  std::string code2 = R"(
  x = load(entry_arg, 0);
  y = load(entry_arg, 1);
  r = x - y;
  store(output, 1, r);
  )";
  std::string code3 = R"(
  x = load(entry_arg, 2);
  y = load(entry_arg, 3);
  r = x * y;
  store(output, 2, r);
  )";
  std::string code4 = R"(
  x = load(entry_arg, 2);
  y = load(entry_arg, 3);
  r = x / y;
  store(output, 3, r);
  )";

  std::vector<std::unique_ptr<ExecNode>> pipelines;
  pipelines.emplace_back(BuildPipeline(code1));
  pipelines.emplace_back(BuildPipeline(code2));
  pipelines.emplace_back(BuildPipeline(code3));
  pipelines.emplace_back(BuildPipeline(code4));

  PipelineGrouper grouper;
  auto groups = grouper.Group(pipelines);

  ASSERT_EQ(groups.size(), 2);
  // Each group should have 2 children
  for (const auto& group : groups) {
    EXPECT_TRUE(IsIsolated(group));
    EXPECT_EQ(GetChildCount(group), 2);
  }
}

// Three pipelines all loading from different indices: 3 groups, each with 1 child.
TEST(PipelineGrouperGroupTest, ThreePipelinesAllDifferentThreeGroups) {
  std::string code1 = R"(
  a = load(entry_arg, 0);
  r = a + 1.0f32;
  store(output, 0, r);
  )";
  std::string code2 = R"(
  a = load(entry_arg, 1);
  r = a + 2.0f32;
  store(output, 1, r);
  )";
  std::string code3 = R"(
  a = load(entry_arg, 2);
  r = a + 3.0f32;
  store(output, 2, r);
  )";

  std::vector<std::unique_ptr<ExecNode>> pipelines;
  pipelines.emplace_back(BuildPipeline(code1));
  pipelines.emplace_back(BuildPipeline(code2));
  pipelines.emplace_back(BuildPipeline(code3));

  PipelineGrouper grouper;
  auto groups = grouper.Group(pipelines);

  ASSERT_EQ(groups.size(), 3);
  for (const auto& group : groups) {
    EXPECT_TRUE(IsIsolated(group));
    EXPECT_EQ(GetChildCount(group), 1);
  }
}

// Partial overlap: code1 loads (0,1), code2 loads (1,2), code3 loads (0,1).
// code1 and code2 share load(entry_arg,1), so they merge.
// code3 also shares load(entry_arg,0) with code1, so all three merge into 1 group.
TEST(PipelineGrouperGroupTest, PartialOverlapAllMerged) {
  std::string code1 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a + b;
  store(output, 0, r);
  )";
  std::string code2 = R"(
  a = load(entry_arg, 1);
  b = load(entry_arg, 2);
  r = a - b;
  store(output, 1, r);
  )";
  std::string code3 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a * b;
  store(output, 2, r);
  )";

  std::vector<std::unique_ptr<ExecNode>> pipelines;
  pipelines.emplace_back(BuildPipeline(code1));
  pipelines.emplace_back(BuildPipeline(code2));
  pipelines.emplace_back(BuildPipeline(code3));

  PipelineGrouper grouper;
  auto groups = grouper.Group(pipelines);

  // code1 and code3 share load(0) and load(1), code1 and code2 share load(1)
  // So all three should be merged into 1 group.
  ASSERT_EQ(groups.size(), 1);
  EXPECT_EQ(GetChildCount(groups[0]), 3);
}

// Chain overlap: code1 loads (0), code2 loads (0,1), code3 loads (1,2), code4 loads (3).
// code1-code2 share load(0), code2-code3 share load(1) => code1,2,3 merge.
// code4 is independent => 2 groups.
TEST(PipelineGrouperGroupTest, ChainOverlapTwoGroups) {
  std::string code1 = R"(
  a = load(entry_arg, 0);
  r = a * 2.0f32;
  store(output, 0, r);
  )";
  std::string code2 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a + b;
  store(output, 1, r);
  )";
  std::string code3 = R"(
  a = load(entry_arg, 1);
  b = load(entry_arg, 2);
  r = a - b;
  store(output, 2, r);
  )";
  std::string code4 = R"(
  a = load(entry_arg, 3);
  r = a + 10.0f32;
  store(output, 3, r);
  )";

  std::vector<std::unique_ptr<ExecNode>> pipelines;
  pipelines.emplace_back(BuildPipeline(code1));
  pipelines.emplace_back(BuildPipeline(code2));
  pipelines.emplace_back(BuildPipeline(code3));
  pipelines.emplace_back(BuildPipeline(code4));

  PipelineGrouper grouper;
  auto groups = grouper.Group(pipelines);

  ASSERT_EQ(groups.size(), 2);
  // One group has 3 children (code1,2,3), the other has 1 (code4)
  size_t total = 0;
  for (const auto& group : groups) {
    EXPECT_TRUE(IsIsolated(group));
    total += GetChildCount(group);
  }
  EXPECT_EQ(total, 4);
  // Find the group sizes
  std::vector<size_t> sizes;
  sizes.reserve(groups.size());
  for (const auto& group : groups) {
    sizes.push_back(GetChildCount(group));
  }
  std::sort(sizes.begin(), sizes.end());
  EXPECT_EQ(sizes[0], 1);
  EXPECT_EQ(sizes[1], 3);
}

// Pipelines with when blocks sharing same load indices should merge.
TEST(PipelineGrouperGroupTest, WhenBlocksSameLoadMerged) {
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

  std::vector<std::unique_ptr<ExecNode>> pipelines;
  pipelines.emplace_back(BuildPipeline(code1));
  pipelines.emplace_back(BuildPipeline(code2));

  PipelineGrouper grouper;
  auto groups = grouper.Group(pipelines);

  ASSERT_EQ(groups.size(), 1);
  EXPECT_EQ(GetChildCount(groups[0]), 2);
}

// Pipelines with when blocks loading from different indices should separate.
TEST(PipelineGrouperGroupTest, WhenBlocksDifferentLoadSeparated) {
  std::string code1 = R"(
  a = load(entry_arg, 0);
  r = 0.0f32;
  when a > 5.0f32 {
    r = a * 2.0f32;
  }
  store(output, 0, r);
  )";
  std::string code2 = R"(
  a = load(entry_arg, 1);
  r = 0.0f32;
  when a > 5.0f32 {
    r = a * 3.0f32;
  }
  store(output, 1, r);
  )";

  std::vector<std::unique_ptr<ExecNode>> pipelines;
  pipelines.emplace_back(BuildPipeline(code1));
  pipelines.emplace_back(BuildPipeline(code2));

  PipelineGrouper grouper;
  auto groups = grouper.Group(pipelines);

  ASSERT_EQ(groups.size(), 2);
  EXPECT_EQ(GetChildCount(groups[0]), 1);
  EXPECT_EQ(GetChildCount(groups[1]), 1);
}

// Five pipelines: 3 share load(0,1), 2 share load(2,3) => 2 groups (3+2).
TEST(PipelineGrouperGroupTest, FivePipelinesTwoUnevenGroups) {
  std::string code1 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a + b;
  store(output, 0, r);
  )";
  std::string code2 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a - b;
  store(output, 1, r);
  )";
  std::string code3 = R"(
  a = load(entry_arg, 0);
  b = load(entry_arg, 1);
  r = a * b;
  store(output, 2, r);
  )";
  std::string code4 = R"(
  a = load(entry_arg, 2);
  b = load(entry_arg, 3);
  r = a + b;
  store(output, 3, r);
  )";
  std::string code5 = R"(
  a = load(entry_arg, 2);
  b = load(entry_arg, 3);
  r = a - b;
  store(output, 4, r);
  )";

  std::vector<std::unique_ptr<ExecNode>> pipelines;
  pipelines.emplace_back(BuildPipeline(code1));
  pipelines.emplace_back(BuildPipeline(code2));
  pipelines.emplace_back(BuildPipeline(code3));
  pipelines.emplace_back(BuildPipeline(code4));
  pipelines.emplace_back(BuildPipeline(code5));

  PipelineGrouper grouper;
  auto groups = grouper.Group(pipelines);

  ASSERT_EQ(groups.size(), 2);
  std::vector<size_t> sizes;
  for (const auto& group : groups) {
    EXPECT_TRUE(IsIsolated(group));
    sizes.push_back(GetChildCount(group));
  }
  std::sort(sizes.begin(), sizes.end());
  EXPECT_EQ(sizes[0], 2);
  EXPECT_EQ(sizes[1], 3);
}

// RefNode chain: same variable names but different load indices should NOT merge.
TEST(PipelineGrouperGroupTest, RefChainDifferentLoadNotMerged) {
  std::string code1 = R"(
  a = load(entry_arg, 0);
  b = a + 1.0f32;
  c = b * 2.0f32;
  store(output, 0, c);
  )";
  std::string code2 = R"(
  a = load(entry_arg, 1);
  b = a + 1.0f32;
  c = b * 2.0f32;
  store(output, 1, c);
  )";

  std::vector<std::unique_ptr<ExecNode>> pipelines;
  pipelines.emplace_back(BuildPipeline(code1));
  pipelines.emplace_back(BuildPipeline(code2));

  PipelineGrouper grouper;
  auto groups = grouper.Group(pipelines);

  ASSERT_EQ(groups.size(), 2);
  EXPECT_EQ(GetChildCount(groups[0]), 1);
  EXPECT_EQ(GetChildCount(groups[1]), 1);
}

/*
 * @Author: victorika
 * @Date: 2026-05-06 11:20:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-06 11:20:00
 */
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "exec_engine.h"
#include "exec_node.h"
#include "function_registry.h"
#include "gtest/gtest.h"
#include "status.h"
#include "type.h"

using namespace jitfusion;

namespace {

constexpr uint32_t kMiss = std::numeric_limits<uint32_t>::max();

// Build a FunctionNode that takes two lists and an ExecContextNode.
std::unique_ptr<ExecNode> MakeLookup(std::unique_ptr<ExecNode> a, std::unique_ptr<ExecNode> b) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(a));
  args.emplace_back(std::move(b));
  args.emplace_back(std::make_unique<ExecContextNode>());
  return std::make_unique<FunctionNode>("ListLookupIndex", std::move(args));
}

std::unique_ptr<ExecNode> MakeCompactPositions(std::unique_ptr<ExecNode> raw) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(raw));
  args.emplace_back(std::make_unique<ExecContextNode>());
  return std::make_unique<FunctionNode>("ListCompactPositions", std::move(args));
}

std::unique_ptr<ExecNode> MakeCompactIndex(std::unique_ptr<ExecNode> raw) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(raw));
  args.emplace_back(std::make_unique<ExecContextNode>());
  return std::make_unique<FunctionNode>("ListCompactIndex", std::move(args));
}

std::unique_ptr<ExecNode> MakeGather(std::unique_ptr<ExecNode> values, std::unique_ptr<ExecNode> idx,
                                     std::unique_ptr<ExecNode> default_value) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(values));
  args.emplace_back(std::move(idx));
  args.emplace_back(std::move(default_value));
  args.emplace_back(std::make_unique<ExecContextNode>());
  return std::make_unique<FunctionNode>("ListGather", std::move(args));
}

RetType RunExpr(std::unique_ptr<ExecNode> root, Status *status_out = nullptr) {
  std::unique_ptr<FunctionRegistry> func_registry;
  EXPECT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  ExecEngine engine;
  EXPECT_TRUE(engine.Compile(root, func_registry).ok());
  RetType result;
  Status st = engine.Execute(nullptr, &result);
  if (status_out != nullptr) {
    *status_out = std::move(st);
  } else {
    EXPECT_TRUE(st.ok());
  }
  return result;
}

}  // namespace

TEST(ListIndexingTest, LookupIndexI32Basic) {
  std::vector<int32_t> a = {10, 20, 30, 40};
  std::vector<int32_t> b = {30, 10, 99};
  auto node = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
  auto result = RunExpr(std::move(node));
  std::vector<uint32_t> expect = {1, kMiss, 0, kMiss};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(ListIndexingTest, LookupIndexI32AllHit) {
  std::vector<int32_t> a = {1, 2, 3};
  std::vector<int32_t> b = {3, 2, 1};
  auto node = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
  auto result = RunExpr(std::move(node));
  std::vector<uint32_t> expect = {2, 1, 0};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(ListIndexingTest, LookupIndexI32AllMiss) {
  std::vector<int32_t> a = {1, 2, 3};
  std::vector<int32_t> b = {4, 5, 6};
  auto node = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
  auto result = RunExpr(std::move(node));
  std::vector<uint32_t> expect = {kMiss, kMiss, kMiss};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(ListIndexingTest, LookupIndexI32EmptyA) {
  std::vector<int32_t> a = {};
  std::vector<int32_t> b = {1, 2};
  auto node = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
  auto result = RunExpr(std::move(node));
  EXPECT_TRUE(std::get<std::vector<uint32_t>>(result).empty());
}

TEST(ListIndexingTest, LookupIndexI32EmptyB) {
  std::vector<int32_t> a = {1, 2};
  std::vector<int32_t> b = {};
  auto node = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
  auto result = RunExpr(std::move(node));
  std::vector<uint32_t> expect = {kMiss, kMiss};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(ListIndexingTest, LookupIndexI32DuplicateBEarliestWins) {
  std::vector<int32_t> a = {7, 8};
  std::vector<int32_t> b = {7, 7, 8, 8};
  auto node = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
  auto result = RunExpr(std::move(node));
  std::vector<uint32_t> expect = {0, 2};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(ListIndexingTest, LookupIndexU64) {
  std::vector<uint64_t> a = {100, 200, 300};
  std::vector<uint64_t> b = {200, 300};
  auto node = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
  auto result = RunExpr(std::move(node));
  std::vector<uint32_t> expect = {kMiss, 0, 1};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(ListIndexingTest, LookupIndexF64) {
  std::vector<double> a = {1.5, 2.5, 3.5};
  std::vector<double> b = {3.5, 1.5};
  auto node = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
  auto result = RunExpr(std::move(node));
  std::vector<uint32_t> expect = {1, kMiss, 0};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(ListIndexingTest, LookupIndexString) {
  std::vector<std::string> a = {"alice", "bob", "carol", "dave"};
  std::vector<std::string> b = {"carol", "alice"};
  auto node = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
  auto result = RunExpr(std::move(node));
  std::vector<uint32_t> expect = {1, kMiss, 0, kMiss};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(ListIndexingTest, CompactPositionsBasic) {
  // raw = [3, MAX, 0, MAX, 2] -> positions = [0, 2, 4]
  std::vector<uint32_t> raw = {3, kMiss, 0, kMiss, 2};
  auto node = MakeCompactPositions(std::make_unique<ConstantListValueNode>(raw));
  auto result = RunExpr(std::move(node));
  std::vector<uint32_t> expect = {0, 2, 4};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(ListIndexingTest, CompactPositionsNoMiss) {
  std::vector<uint32_t> raw = {5, 3, 2};
  auto node = MakeCompactPositions(std::make_unique<ConstantListValueNode>(raw));
  auto result = RunExpr(std::move(node));
  std::vector<uint32_t> expect = {0, 1, 2};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(ListIndexingTest, CompactPositionsAllMiss) {
  std::vector<uint32_t> raw = {kMiss, kMiss, kMiss};
  auto node = MakeCompactPositions(std::make_unique<ConstantListValueNode>(raw));
  auto result = RunExpr(std::move(node));
  EXPECT_TRUE(std::get<std::vector<uint32_t>>(result).empty());
}

TEST(ListIndexingTest, CompactPositionsEmpty) {
  std::vector<uint32_t> raw = {};
  auto node = MakeCompactPositions(std::make_unique<ConstantListValueNode>(raw));
  auto result = RunExpr(std::move(node));
  EXPECT_TRUE(std::get<std::vector<uint32_t>>(result).empty());
}

TEST(ListIndexingTest, CompactIndexBasic) {
  // raw = [3, MAX, 0, MAX, 2] -> compact_idx = [3, 0, 2]
  std::vector<uint32_t> raw = {3, kMiss, 0, kMiss, 2};
  auto node = MakeCompactIndex(std::make_unique<ConstantListValueNode>(raw));
  auto result = RunExpr(std::move(node));
  std::vector<uint32_t> expect = {3, 0, 2};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(ListIndexingTest, CompactIndexNoMiss) {
  std::vector<uint32_t> raw = {5, 3, 2};
  auto node = MakeCompactIndex(std::make_unique<ConstantListValueNode>(raw));
  auto result = RunExpr(std::move(node));
  std::vector<uint32_t> expect = {5, 3, 2};
  EXPECT_EQ(std::get<std::vector<uint32_t>>(result), expect);
}

TEST(ListIndexingTest, GatherI32WithDefault) {
  // values = [100, 200, 300]
  // idx    = [2, MAX, 0, 1, MAX]
  // out    = [300, -1, 100, 200, -1]
  std::vector<int32_t> values = {100, 200, 300};
  std::vector<uint32_t> idx = {2, kMiss, 0, 1, kMiss};
  auto node = MakeGather(std::make_unique<ConstantListValueNode>(values), std::make_unique<ConstantListValueNode>(idx),
                         std::make_unique<ConstantValueNode>(static_cast<int32_t>(-1)));
  auto result = RunExpr(std::move(node));
  std::vector<int32_t> expect = {300, -1, 100, 200, -1};
  EXPECT_EQ(std::get<std::vector<int32_t>>(result), expect);
}

TEST(ListIndexingTest, GatherF64WithDefault) {
  std::vector<double> values = {1.1, 2.2, 3.3};
  std::vector<uint32_t> idx = {kMiss, 2, 0};
  auto node = MakeGather(std::make_unique<ConstantListValueNode>(values), std::make_unique<ConstantListValueNode>(idx),
                         std::make_unique<ConstantValueNode>((-9.9)));
  auto result = RunExpr(std::move(node));
  std::vector<double> expect = {-9.9, 3.3, 1.1};
  EXPECT_EQ(std::get<std::vector<double>>(result), expect);
}

TEST(ListIndexingTest, GatherStringWithDefault) {
  std::vector<std::string> values = {"foo", "bar", "baz"};
  std::vector<uint32_t> idx = {1, kMiss, 0};
  auto node = MakeGather(std::make_unique<ConstantListValueNode>(values), std::make_unique<ConstantListValueNode>(idx),
                         std::make_unique<ConstantValueNode>(std::string("<none>")));
  auto result = RunExpr(std::move(node));
  std::vector<std::string> expect = {"bar", "<none>", "foo"};
  EXPECT_EQ(std::get<std::vector<std::string>>(result), expect);
}

TEST(ListIndexingTest, GatherEmptyIdx) {
  std::vector<int32_t> values = {1, 2, 3};
  std::vector<uint32_t> idx = {};
  auto node = MakeGather(std::make_unique<ConstantListValueNode>(values), std::make_unique<ConstantListValueNode>(idx),
                         std::make_unique<ConstantValueNode>(static_cast<int32_t>(0)));
  auto result = RunExpr(std::move(node));
  EXPECT_TRUE(std::get<std::vector<int32_t>>(result).empty());
}

// -----------------------------------------------------------------------------
// End-to-end join scenarios mirroring the motivating use case:
//   a (keys) - c (a-side value)       // one-to-one
//   b (keys) - d1, d2 (b-side values) // one-to-one
// "Join on key, return d1/d2 aligned to a".
// -----------------------------------------------------------------------------

TEST(ListIndexingTest, E2EJoinWithDefaultTwoValueColumns) {
  // a = [10, 20, 30, 40]  (key)
  // b = [30, 10, 99]      (key)
  // d1 aligned to b: [300, 100, 999]
  // d2 aligned to b: ["c30", "c10", "c99"]
  // Expected, aligned to a with defaults:
  //   d1' = [100, 0, 300, 0]
  //   d2' = ["c10", "NA", "c30", "NA"]
  std::vector<int32_t> a = {10, 20, 30, 40};
  std::vector<int32_t> b = {30, 10, 99};
  std::vector<int32_t> d1 = {300, 100, 999};
  std::vector<std::string> d2 = {"c30", "c10", "c99"};

  // raw = ListLookupIndex(a, b)
  auto raw_node = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
  // ListGather(d1, raw, 0)
  auto gather_d1 = MakeGather(std::make_unique<ConstantListValueNode>(d1), std::move(raw_node),
                              std::make_unique<ConstantValueNode>(static_cast<int32_t>(0)));
  auto r1 = RunExpr(std::move(gather_d1));
  std::vector<int32_t> expect_d1 = {100, 0, 300, 0};
  EXPECT_EQ(std::get<std::vector<int32_t>>(r1), expect_d1);

  // Redo the lookup (CSE would merge in real expressions; tests run each tree
  // standalone).
  auto raw_node2 = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
  auto gather_d2 = MakeGather(std::make_unique<ConstantListValueNode>(d2), std::move(raw_node2),
                              std::make_unique<ConstantValueNode>(std::string("NA")));
  auto r2 = RunExpr(std::move(gather_d2));
  std::vector<std::string> expect_d2 = {"c10", "NA", "c30", "NA"};
  EXPECT_EQ(std::get<std::vector<std::string>>(r2), expect_d2);
}

TEST(ListIndexingTest, E2EJoinFilteredBSideColumnOnly) {
  // Only b-side columns requested -> single hash pass.
  // a = [10, 20, 30, 40], b = [30, 10], d = [300, 100].
  // Filtered output (b-side): [100, 300]  (order follows a's hit sequence).
  std::vector<int32_t> a = {10, 20, 30, 40};
  std::vector<int32_t> b = {30, 10};
  std::vector<int32_t> d = {300, 100};

  auto raw = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
  auto bidx = MakeCompactIndex(std::move(raw));
  auto gather = MakeGather(std::make_unique<ConstantListValueNode>(d), std::move(bidx),
                           std::make_unique<ConstantValueNode>(static_cast<int32_t>(0)));
  auto r = RunExpr(std::move(gather));
  std::vector<int32_t> expect = {100, 300};
  EXPECT_EQ(std::get<std::vector<int32_t>>(r), expect);
}

TEST(ListIndexingTest, E2EJoinFilteredBothSides) {
  // Full inner-join shape: keep both a-side (a, c) and b-side (d) columns for
  // rows where a[i] matches something in b.
  //   a = [10, 20, 30, 40],       c = [11, 22, 33, 44]  (a-side)
  //   b = [30, 10],                d = [300, 100]        (b-side)
  // Hits (in a-order): i=0 (a=10 -> b[1]), i=2 (a=30 -> b[0]).
  // Expected:
  //   a_out = [10, 30]
  //   c_out = [11, 33]
  //   d_out = [100, 300]
  std::vector<int32_t> a = {10, 20, 30, 40};
  std::vector<int32_t> c = {11, 22, 33, 44};
  std::vector<int32_t> b = {30, 10};
  std::vector<int32_t> d = {300, 100};

  // a-side: ListGather(a, ListCompactPositions(ListLookupIndex(a, b)), 0)
  {
    auto raw = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
    auto aidx = MakeCompactPositions(std::move(raw));
    auto gather = MakeGather(std::make_unique<ConstantListValueNode>(a), std::move(aidx),
                             std::make_unique<ConstantValueNode>(static_cast<int32_t>(0)));
    auto r = RunExpr(std::move(gather));
    std::vector<int32_t> expect = {10, 30};
    EXPECT_EQ(std::get<std::vector<int32_t>>(r), expect);
  }
  // c-side: same aidx pattern, different values list.
  {
    auto raw = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
    auto aidx = MakeCompactPositions(std::move(raw));
    auto gather = MakeGather(std::make_unique<ConstantListValueNode>(c), std::move(aidx),
                             std::make_unique<ConstantValueNode>(static_cast<int32_t>(0)));
    auto r = RunExpr(std::move(gather));
    std::vector<int32_t> expect = {11, 33};
    EXPECT_EQ(std::get<std::vector<int32_t>>(r), expect);
  }
  // d-side: ListGather(d, ListCompactIndex(ListLookupIndex(a, b)), 0)
  {
    auto raw = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
    auto bidx = MakeCompactIndex(std::move(raw));
    auto gather = MakeGather(std::make_unique<ConstantListValueNode>(d), std::move(bidx),
                             std::make_unique<ConstantValueNode>(static_cast<int32_t>(0)));
    auto r = RunExpr(std::move(gather));
    std::vector<int32_t> expect = {100, 300};
    EXPECT_EQ(std::get<std::vector<int32_t>>(r), expect);
  }
}

TEST(ListIndexingTest, E2EJoinFilteredStringKeys) {
  // Keys are strings; values can be any type.
  std::vector<std::string> a = {"alice", "bob", "carol", "dave"};
  std::vector<std::string> b = {"carol", "alice"};
  std::vector<int32_t> d = {333, 111};

  auto raw = MakeLookup(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantListValueNode>(b));
  auto bidx = MakeCompactIndex(std::move(raw));
  auto gather = MakeGather(std::make_unique<ConstantListValueNode>(d), std::move(bidx),
                           std::make_unique<ConstantValueNode>(static_cast<int32_t>(0)));
  auto r = RunExpr(std::move(gather));
  std::vector<int32_t> expect = {111, 333};  // alice -> 111, carol -> 333
  EXPECT_EQ(std::get<std::vector<int32_t>>(r), expect);
}

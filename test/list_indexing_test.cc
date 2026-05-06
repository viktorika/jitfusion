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

std::unique_ptr<ExecNode> MakeFind(std::unique_ptr<ExecNode> a, std::unique_ptr<ExecNode> value) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(a));
  args.emplace_back(std::move(value));
  return std::make_unique<FunctionNode>("Find", std::move(args));
}

std::unique_ptr<ExecNode> MakeFindSorted(std::unique_ptr<ExecNode> a, std::unique_ptr<ExecNode> value) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(a));
  args.emplace_back(std::move(value));
  return std::make_unique<FunctionNode>("FindSorted", std::move(args));
}

std::unique_ptr<ExecNode> MakeFindMiss() {
  return std::make_unique<FunctionNode>("FindMiss", std::vector<std::unique_ptr<ExecNode>>{});
}

std::unique_ptr<ExecNode> MakeGetAt(std::unique_ptr<ExecNode> values, std::unique_ptr<ExecNode> index) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::move(values));
  args.emplace_back(std::move(index));
  return std::make_unique<FunctionNode>("GetAt", std::move(args));
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

// -----------------------------------------------------------------------------
// Find: return the index of the first occurrence of `value` in the list.
// Miss -> std::numeric_limits<uint32_t>::max() (kMiss), which naturally
// composes with GetAt (out-of-bounds -> zero-valued default).
// -----------------------------------------------------------------------------

TEST(ListIndexingTest, FindI32Basic) {
  std::vector<int32_t> a = {10, 20, 30, 40};
  auto node = MakeFind(std::make_unique<ConstantListValueNode>(a),
                       std::make_unique<ConstantValueNode>(static_cast<int32_t>(30)));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), 2U);
}

TEST(ListIndexingTest, FindI32Miss) {
  std::vector<int32_t> a = {10, 20, 30};
  auto node = MakeFind(std::make_unique<ConstantListValueNode>(a),
                       std::make_unique<ConstantValueNode>(static_cast<int32_t>(99)));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), kMiss);
}

TEST(ListIndexingTest, FindI32DuplicateReturnsFirst) {
  std::vector<int32_t> a = {7, 8, 7, 7};
  auto node = MakeFind(std::make_unique<ConstantListValueNode>(a),
                       std::make_unique<ConstantValueNode>(static_cast<int32_t>(7)));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), 0U);
}

TEST(ListIndexingTest, FindEmptyList) {
  std::vector<int32_t> a = {};
  auto node = MakeFind(std::make_unique<ConstantListValueNode>(a),
                       std::make_unique<ConstantValueNode>(static_cast<int32_t>(1)));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), kMiss);
}

TEST(ListIndexingTest, FindU64) {
  std::vector<uint64_t> a = {100, 200, 300};
  auto node = MakeFind(std::make_unique<ConstantListValueNode>(a),
                       std::make_unique<ConstantValueNode>(static_cast<uint64_t>(300)));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), 2U);
}

TEST(ListIndexingTest, FindF64) {
  std::vector<double> a = {1.5, 2.5, 3.5};
  auto node = MakeFind(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantValueNode>(2.5));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), 1U);
}

TEST(ListIndexingTest, FindString) {
  std::vector<std::string> a = {"alice", "bob", "carol"};
  auto node =
      MakeFind(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantValueNode>(std::string("carol")));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), 2U);
}

TEST(ListIndexingTest, FindStringMiss) {
  std::vector<std::string> a = {"alice", "bob"};
  auto node =
      MakeFind(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantValueNode>(std::string("zoe")));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), kMiss);
}

// E2E: scalar join == GetAt(values, Find(keys, k))
// Hit: returns the matched value; Miss: Find -> kMiss, GetAt -> zero-valued
// default (0 for ints). This demonstrates the intended composition.
TEST(ListIndexingTest, E2EScalarJoinHit) {
  std::vector<int32_t> keys = {10, 20, 30};
  std::vector<int32_t> values = {100, 200, 300};
  auto find = MakeFind(std::make_unique<ConstantListValueNode>(keys),
                       std::make_unique<ConstantValueNode>(static_cast<int32_t>(20)));
  auto getat = MakeGetAt(std::make_unique<ConstantListValueNode>(values), std::move(find));
  auto result = RunExpr(std::move(getat));
  EXPECT_EQ(std::get<int32_t>(result), 200);
}

TEST(ListIndexingTest, E2EScalarJoinMiss) {
  std::vector<int32_t> keys = {10, 20, 30};
  std::vector<int32_t> values = {100, 200, 300};
  auto find = MakeFind(std::make_unique<ConstantListValueNode>(keys),
                       std::make_unique<ConstantValueNode>(static_cast<int32_t>(99)));
  auto getat = MakeGetAt(std::make_unique<ConstantListValueNode>(values), std::move(find));
  auto result = RunExpr(std::move(getat));
  // Find returns kMiss -> GetAt goes out-of-bounds -> returns 0 for int32.
  EXPECT_EQ(std::get<int32_t>(result), 0);
}

// -----------------------------------------------------------------------------
// FindSorted: O(log n) binary search variant of Find. The caller MUST guarantee
// the list is sorted in ascending order; otherwise the result is unspecified
// (same contract as std::lower_bound). On miss returns kMiss.
// -----------------------------------------------------------------------------

TEST(ListIndexingTest, FindSortedI32MidHit) {
  std::vector<int32_t> a = {10, 20, 30, 40, 50};
  auto node = MakeFindSorted(std::make_unique<ConstantListValueNode>(a),
                             std::make_unique<ConstantValueNode>(static_cast<int32_t>(30)));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), 2U);
}

TEST(ListIndexingTest, FindSortedI32LeftBoundary) {
  std::vector<int32_t> a = {10, 20, 30};
  auto node = MakeFindSorted(std::make_unique<ConstantListValueNode>(a),
                             std::make_unique<ConstantValueNode>(static_cast<int32_t>(10)));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), 0U);
}

TEST(ListIndexingTest, FindSortedI32RightBoundary) {
  std::vector<int32_t> a = {10, 20, 30};
  auto node = MakeFindSorted(std::make_unique<ConstantListValueNode>(a),
                             std::make_unique<ConstantValueNode>(static_cast<int32_t>(30)));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), 2U);
}

TEST(ListIndexingTest, FindSortedI32MissBelow) {
  std::vector<int32_t> a = {10, 20, 30};
  auto node = MakeFindSorted(std::make_unique<ConstantListValueNode>(a),
                             std::make_unique<ConstantValueNode>(static_cast<int32_t>(5)));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), kMiss);
}

TEST(ListIndexingTest, FindSortedI32MissAbove) {
  std::vector<int32_t> a = {10, 20, 30};
  auto node = MakeFindSorted(std::make_unique<ConstantListValueNode>(a),
                             std::make_unique<ConstantValueNode>(static_cast<int32_t>(99)));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), kMiss);
}

TEST(ListIndexingTest, FindSortedI32MissInGap) {
  std::vector<int32_t> a = {10, 20, 30};
  auto node = MakeFindSorted(std::make_unique<ConstantListValueNode>(a),
                             std::make_unique<ConstantValueNode>(static_cast<int32_t>(15)));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), kMiss);
}

TEST(ListIndexingTest, FindSortedEmptyList) {
  std::vector<int32_t> a = {};
  auto node = MakeFindSorted(std::make_unique<ConstantListValueNode>(a),
                             std::make_unique<ConstantValueNode>(static_cast<int32_t>(1)));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), kMiss);
}

TEST(ListIndexingTest, FindSortedI32DuplicatesReturnsFirst) {
  // std::lower_bound naturally returns the first equal element.
  std::vector<int32_t> a = {10, 20, 20, 20, 30};
  auto node = MakeFindSorted(std::make_unique<ConstantListValueNode>(a),
                             std::make_unique<ConstantValueNode>(static_cast<int32_t>(20)));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), 1U);
}

TEST(ListIndexingTest, FindSortedU64) {
  std::vector<uint64_t> a = {100, 200, 300, 400};
  auto node = MakeFindSorted(std::make_unique<ConstantListValueNode>(a),
                             std::make_unique<ConstantValueNode>(static_cast<uint64_t>(300)));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), 2U);
}

TEST(ListIndexingTest, FindSortedF64) {
  std::vector<double> a = {1.5, 2.5, 3.5};
  auto node = MakeFindSorted(std::make_unique<ConstantListValueNode>(a), std::make_unique<ConstantValueNode>(2.5));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), 1U);
}

TEST(ListIndexingTest, FindSortedStringHit) {
  // Strings must be in lexicographic ascending order.
  std::vector<std::string> a = {"alice", "bob", "carol", "dave"};
  auto node = MakeFindSorted(std::make_unique<ConstantListValueNode>(a),
                             std::make_unique<ConstantValueNode>(std::string("carol")));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), 2U);
}

TEST(ListIndexingTest, FindSortedStringMiss) {
  std::vector<std::string> a = {"alice", "bob", "carol"};
  auto node = MakeFindSorted(std::make_unique<ConstantListValueNode>(a),
                             std::make_unique<ConstantValueNode>(std::string("zoe")));
  auto result = RunExpr(std::move(node));
  EXPECT_EQ(std::get<uint32_t>(result), kMiss);
}

// -----------------------------------------------------------------------------
// FindMiss: zero-arg constant that equals the lookup-miss sentinel. Intended
// for DSL-side use so users can write `Find(keys, k) != FindMiss()`.
// -----------------------------------------------------------------------------

TEST(ListIndexingTest, FindMissEqualsKMiss) {
  auto result = RunExpr(MakeFindMiss());
  EXPECT_EQ(std::get<uint32_t>(result), kMiss);
}

TEST(ListIndexingTest, FindMissMatchesFindMiss) {
  // Find miss path must return the same sentinel as FindMiss().
  std::vector<int32_t> a = {10, 20, 30};
  auto find = MakeFind(std::make_unique<ConstantListValueNode>(a),
                       std::make_unique<ConstantValueNode>(static_cast<int32_t>(99)));
  auto find_result = RunExpr(std::move(find));
  auto miss_result = RunExpr(MakeFindMiss());
  EXPECT_EQ(std::get<uint32_t>(find_result), std::get<uint32_t>(miss_result));
}

TEST(ListIndexingTest, FindMissMatchesFindSortedMiss) {
  std::vector<int32_t> a = {10, 20, 30};
  auto find = MakeFindSorted(std::make_unique<ConstantListValueNode>(a),
                             std::make_unique<ConstantValueNode>(static_cast<int32_t>(99)));
  auto find_result = RunExpr(std::move(find));
  auto miss_result = RunExpr(MakeFindMiss());
  EXPECT_EQ(std::get<uint32_t>(find_result), std::get<uint32_t>(miss_result));
}

// ---- Bucketize tests -------------------------------------------------------

namespace {

// Build a Bucketize(values, boundaries, exec_ctx) FunctionNode.
template <typename T>
std::unique_ptr<ExecNode> MakeBucketize(const std::vector<T>& values, const std::vector<T>& boundaries) {
  std::vector<std::unique_ptr<ExecNode>> args;
  args.emplace_back(std::make_unique<ConstantListValueNode>(values));
  args.emplace_back(std::make_unique<ConstantListValueNode>(boundaries));
  args.emplace_back(std::make_unique<ExecContextNode>());
  return std::make_unique<FunctionNode>("Bucketize", std::move(args));
}

template <typename T>
std::vector<uint32_t> RunBucketize(const std::vector<T>& values, const std::vector<T>& boundaries) {
  auto result = RunExpr(MakeBucketize(values, boundaries));
  return std::get<std::vector<uint32_t>>(result);
}

}  // namespace

TEST(ListIndexingTest, BucketizeBasicI32) {
  // boundaries = [10, 20, 30] -> 4 buckets: (-inf,10) [10,20) [20,30) [30,+inf)
  std::vector<int32_t> values = {5, 10, 15, 20, 25, 30, 35};
  std::vector<int32_t> boundaries = {10, 20, 30};
  std::vector<uint32_t> expect = {0, 1, 1, 2, 2, 3, 3};
  EXPECT_EQ(RunBucketize(values, boundaries), expect);
}

TEST(ListIndexingTest, BucketizeLeftClosedRightOpen) {
  // Element exactly equal to a boundary lands in the bucket starting at it
  // (left-closed). Element just below stays in the previous bucket.
  std::vector<int64_t> values = {9, 10, 11, 19, 20, 21};
  std::vector<int64_t> boundaries = {10, 20};
  // Layout: (-inf,10)=0, [10,20)=1, [20,+inf)=2
  std::vector<uint32_t> expect = {0, 1, 1, 1, 2, 2};
  EXPECT_EQ(RunBucketize(values, boundaries), expect);
}

TEST(ListIndexingTest, BucketizeOutOfRangeFallsToEndBuckets) {
  // Below min -> 0; at-or-above max -> len(boundaries).
  std::vector<int32_t> values = {-1000, 0, 100, 1000};
  std::vector<int32_t> boundaries = {0, 100};
  // Layout: (-inf,0)=0, [0,100)=1, [100,+inf)=2
  std::vector<uint32_t> expect = {0, 1, 2, 2};
  EXPECT_EQ(RunBucketize(values, boundaries), expect);
}

TEST(ListIndexingTest, BucketizeEmptyBoundaries) {
  // No boundaries -> single (-inf,+inf) bucket -> everything is 0.
  std::vector<int32_t> values = {-1, 0, 1, 2, 3};
  std::vector<int32_t> boundaries = {};
  std::vector<uint32_t> expect = {0, 0, 0, 0, 0};
  EXPECT_EQ(RunBucketize(values, boundaries), expect);
}

TEST(ListIndexingTest, BucketizeEmptyValues) {
  std::vector<double> values = {};
  std::vector<double> boundaries = {1.0, 2.0, 3.0};
  EXPECT_TRUE(RunBucketize(values, boundaries).empty());
}

TEST(ListIndexingTest, BucketizeFloat) {
  // Verify floating-point ordering and a value sitting exactly on a boundary.
  std::vector<double> values = {0.5, 1.0, 1.5, 2.5, 3.5};
  std::vector<double> boundaries = {1.0, 2.0, 3.0};
  std::vector<uint32_t> expect = {0, 1, 1, 2, 3};
  EXPECT_EQ(RunBucketize(values, boundaries), expect);
}

TEST(ListIndexingTest, BucketizeNaNGoesToTopBucket) {
  // NaN compares false against everything, so std::lower_bound walks past
  // the end and the value is mapped to bucket = len(boundaries). Matches
  // NumPy / PyTorch behaviour.
  const double nan = std::numeric_limits<double>::quiet_NaN();
  std::vector<double> values = {nan, 1.5};
  std::vector<double> boundaries = {1.0, 2.0};
  auto got = RunBucketize(values, boundaries);
  ASSERT_EQ(got.size(), 2u);
  EXPECT_EQ(got[0], 2u);  // NaN -> top bucket
  EXPECT_EQ(got[1], 1u);  // 1.5 -> [1.0, 2.0)
}

TEST(ListIndexingTest, BucketizeUnsigned) {
  // Sanity check on an unsigned numeric type to confirm the template
  // instantiates and registers correctly.
  std::vector<uint64_t> values = {0, 50, 100, 150, 200};
  std::vector<uint64_t> boundaries = {100, 200};
  std::vector<uint32_t> expect = {0, 0, 1, 1, 2};
  EXPECT_EQ(RunBucketize(values, boundaries), expect);
}

// Internally Bucketize switches between a linear scan and std::upper_bound at
// kBucketizeLinearThreshold (=16). Verify both paths produce identical output
// across the threshold boundary, including for the value-equals-boundary case
// that distinguishes upper_bound from lower_bound.
TEST(ListIndexingTest, BucketizeLinearAndBinaryPathsAgree) {
  // Build boundaries 10, 20, ..., 10*N and probe every interesting position:
  // strictly below, exactly on, strictly between, exactly on the next, ...
  auto build_case = [](int n_boundaries) {
    std::vector<int32_t> boundaries;
    boundaries.reserve(n_boundaries);
    for (int i = 1; i <= n_boundaries; ++i) {
      boundaries.push_back(i * 10);
    }
    std::vector<int32_t> values;
    std::vector<uint32_t> expect;
    // below the smallest boundary
    values.push_back(0);
    expect.push_back(0);
    for (int i = 0; i < n_boundaries; ++i) {
      // exactly on the i-th boundary -> bucket i+1 (left-closed)
      values.push_back(boundaries[i]);
      expect.push_back(static_cast<uint32_t>(i + 1));
      // strictly between b[i] and b[i+1] -> bucket i+1
      values.push_back(boundaries[i] + 5);
      expect.push_back(static_cast<uint32_t>(i + 1));
    }
    // way above the top -> bucket n
    values.push_back(10 * n_boundaries + 1000);
    expect.push_back(static_cast<uint32_t>(n_boundaries));
    return std::make_pair(std::move(values), std::pair{std::move(boundaries), std::move(expect)});
  };

  // n=16: hits the linear scan path (last `<=` threshold value).
  // n=17: hits the std::upper_bound path (first value above threshold).
  // n=1 / n=2: tiny edge cases.
  for (int n : {1, 2, 16, 17, 64}) {
    auto [values, rest] = build_case(n);
    auto& [boundaries, expect] = rest;
    EXPECT_EQ(RunBucketize(values, boundaries), expect) << "n=" << n;
  }
}

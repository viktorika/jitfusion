/*
 * @Author: victorika
 * @Date: 2026-05-09 14:40:12
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-11 14:55:10
 */
#include "gtest/gtest.h"
#include "test_helper.h"

using athena::AthenaPipeline;
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
  AthenaPipeline athena;
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
  AthenaPipeline athena;
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
  AthenaPipeline athena;
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
  AthenaPipeline athena;
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
  AthenaPipeline athena;
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
  AthenaPipeline athena;
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
  AthenaPipeline athena;
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
  AthenaPipeline athena;
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

// -----------------------------------------------------------------------------
// CSE for readonly C functions whose return type is kPtr.
//
// The split-kernel design of ListLookupIndex relies on LLVM EarlyCSE/GVN
// merging two same-arg calls of a readonly ptr-returning function (the
// hash-table builder) into one. The tests below pin that behavior down
// at the Athena DSL layer with a runtime call counter — the same pattern
// the CSETests above use for scalar-returning functions.
// -----------------------------------------------------------------------------

namespace {

// Per-pipeline state that doubles as both the entry-arg payload and the
// scratch space the make_ptr helper returns a pointer into. Keeping the
// pointer stable across invocations means use_ptr never has to worry
// about memory ownership; the only side effect we care about is the
// monotonically-increasing call counter.
struct PtrCSETestData {
  int32_t storage{0};
  int32_t make_ptr_count{0};
  int32_t use_ptr_count{0};
};

// Stand-in for BuildHashTable: returns a stable pointer (so that two
// merged calls would observe identical results) and bumps a per-call
// counter so the test can assert how many times the function actually
// ran. The "seed" parameter is what we expect LLVM CSE to key off of.
void* CountedMakePtr(void* entry_arguments, int32_t seed) {
  auto* data = reinterpret_cast<PtrCSETestData*>(entry_arguments);
  data->make_ptr_count++;
  data->storage = seed;
  return &data->storage;
}

// Stand-in for LookupHashTable: dereferences the table and adds k. Two
// calls with different k must NOT be merged.
int32_t CountedUsePtr(void* entry_arguments, void* p, int32_t k) {
  auto* data = reinterpret_cast<PtrCSETestData*>(entry_arguments);
  data->use_ptr_count++;
  return *reinterpret_cast<int32_t*>(p) + k;
}

void StoreI32(void* output, int32_t index, int32_t value) { reinterpret_cast<int32_t*>(output)[index] = value; }

}  // namespace

TEST(CSETest, ReadOnlyPtrReturningFunctionMerge) {
  // Two use_ptr() calls feed off make_ptr(entry_arg, 7) with identical
  // arguments. Because make_ptr is registered readonly+nounwind, LLVM
  // EarlyCSE/GVN must merge the two make_ptr calls into one; the two
  // use_ptr calls take different k and stay separate. The counter assertions
  // below are the source of truth — IR text would be brittle across LLVM
  // upgrades.
  AthenaPipeline athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("make_ptr", {ValueType::kPtr, ValueType::kI32}, ValueType::kPtr);
    ASSERT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedMakePtr)).ok());
  }
  {
    FunctionSignature sign("use_ptr", {ValueType::kPtr, ValueType::kPtr, ValueType::kI32}, ValueType::kI32);
    ASSERT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedUsePtr)).ok());
  }
  {
    FunctionSignature sign("store_i32", {ValueType::kPtr, ValueType::kI32, ValueType::kI32}, ValueType::kVoid);
    ASSERT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreI32), 1).ok());
  }

  // result = use_ptr(make_ptr(entry, 7), 10) + use_ptr(make_ptr(entry, 7), 20)
  // Expected result: (7 + 10) + (7 + 20) = 44
  std::string code = R"(
  p1 = make_ptr(entry_arg, 7);
  p2 = make_ptr(entry_arg, 7);
  v1 = use_ptr(entry_arg, p1, 10);
  v2 = use_ptr(entry_arg, p2, 20);
  store_i32(output, 0, v1 + v2);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  PtrCSETestData data{};
  std::vector<int32_t> output(1, 0);
  ASSERT_TRUE(athena.Execute(&data, output.data()).ok());

  EXPECT_EQ(output[0], 44);

  // Core assertion: same-arg readonly calls returning kPtr collapse to
  // a single runtime invocation.
  EXPECT_EQ(data.make_ptr_count, 1) << "two same-arg make_ptr calls must be merged by LLVM CSE";
  // Differing k keeps the two use_ptr probes distinct.
  EXPECT_EQ(data.use_ptr_count, 2) << "different-arg use_ptr calls must remain separate";
}

TEST(CSETest, ReadOnlyPtrReturningFunctionDifferentArgsNoMerge) {
  // Same shape as above, but make_ptr now takes a different seed each time.
  // CSE must NOT merge — we want each seed to flow through.
  AthenaPipeline athena;
  std::unique_ptr<FunctionRegistry> func_registry;
  ASSERT_TRUE(FunctionRegistryFactory::CreateFunctionRegistry(&func_registry).ok());
  {
    FunctionSignature sign("make_ptr", {ValueType::kPtr, ValueType::kI32}, ValueType::kPtr);
    ASSERT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedMakePtr)).ok());
  }
  {
    FunctionSignature sign("use_ptr", {ValueType::kPtr, ValueType::kPtr, ValueType::kI32}, ValueType::kI32);
    ASSERT_TRUE(func_registry->RegisterReadOnlyCFunc(sign, reinterpret_cast<void*>(CountedUsePtr)).ok());
  }
  {
    FunctionSignature sign("store_i32", {ValueType::kPtr, ValueType::kI32, ValueType::kI32}, ValueType::kVoid);
    ASSERT_TRUE(func_registry->RegisterStoreCFunc(sign, reinterpret_cast<void*>(StoreI32), 1).ok());
  }

  std::string code = R"(
  p1 = make_ptr(entry_arg, 1);
  p2 = make_ptr(entry_arg, 2);
  v1 = use_ptr(entry_arg, p1, 10);
  v2 = use_ptr(entry_arg, p2, 20);
  store_i32(output, 0, v1 + v2);
  )";

  std::vector<std::string> codes = {code};
  ASSERT_TRUE(athena.Compile(codes, func_registry).ok());

  PtrCSETestData data{};
  std::vector<int32_t> output(1, 0);
  ASSERT_TRUE(athena.Execute(&data, output.data()).ok());

  // Note: because both calls return &data.storage, the second make_ptr
  // overwrites storage to its seed before use_ptr reads it, so v1 actually
  // sees the *second* seed by the time it dereferences. Whether that
  // happens depends on the order LLVM schedules the calls; but for the
  // sake of asserting "no merge" we only care about the counter.
  EXPECT_EQ(data.make_ptr_count, 2) << "different-seed make_ptr calls must NOT be merged";
  EXPECT_EQ(data.use_ptr_count, 2);
}

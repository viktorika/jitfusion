/*
 * @Author: victorika
 * @Date: 2025-04-09 15:43:45
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-08 10:00:00
 */
#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "arena.h"
#include "batch_exec_engine.h"
#include "exec_engine.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace athena {

using jitfusion::Arena;
using jitfusion::BatchExecEngine;
using jitfusion::BinaryOPType;
using jitfusion::CodeGenFunc;
using jitfusion::ConstantListValueType;
using jitfusion::ConstantValueType;
using jitfusion::ExecContext;
using jitfusion::ExecEngine;
using jitfusion::ExecEngineOption;
using jitfusion::ExecNodeType;
using jitfusion::F32ListStruct;
using jitfusion::F64ListStruct;
using jitfusion::FunctionRegistry;
using jitfusion::FunctionRegistryFactory;
using jitfusion::FunctionSignature;
using jitfusion::FunctionStructure;
using jitfusion::FunctionType;
using jitfusion::I16ListStruct;
using jitfusion::I32ListStruct;
using jitfusion::I64ListStruct;
using jitfusion::I8ListStruct;
using jitfusion::IRCodeGenContext;
using jitfusion::LLVMStructType;
using jitfusion::RetType;
using jitfusion::Status;
using jitfusion::StringListStruct;
using jitfusion::StringStruct;
using jitfusion::TypeHelper;
using jitfusion::U16ListStruct;
using jitfusion::U32ListStruct;
using jitfusion::U64ListStruct;
using jitfusion::U8ListStruct;
using jitfusion::UnaryOPType;
using jitfusion::ValueType;

// AthenaExpression compiles and runs a single DSL expression. The result
// is returned through the RetType variant. This is the simplest form of
// usage — "one line of athena DSL, one value out".
//
// For multi-statement programs that write data through custom store
// functions (the pipeline form), use AthenaPipeline below.
//
// Thread-safety contract (mirrors ExecEngine):
// - Compile() is NOT thread-safe.
// - After a successful Compile(), the Execute() overloads are safe to
//   invoke concurrently from multiple threads, provided each thread uses
//   its own ExecContext.
// - The Execute() overloads that do not take an ExecContext internally
//   construct a fresh one per call (thread-safe but pay per-call
//   allocation cost). Prefer the ExecContext& overloads on hot paths.
// - A single ExecContext must NOT be shared across threads. Recommended
//   pattern for parallel execution: 1 AthenaExpression + N ExecContexts
//   (one per worker).
class AthenaExpression {
 public:
  explicit AthenaExpression(ExecEngineOption option = {}) : engine_(option) {}
  ~AthenaExpression() = default;
  AthenaExpression(const AthenaExpression&) = delete;
  AthenaExpression& operator=(const AthenaExpression&) = delete;

  // Compile a single DSL expression into an executable entry function.
  Status Compile(const std::string& code, const std::unique_ptr<FunctionRegistry>& func_registry);

  // Run the compiled expression and box the result into RetType.
  Status Execute(void* entry_arguments, RetType* result) { return engine_.Execute(entry_arguments, result); }
  Status Execute(ExecContext& exec_ctx, void* entry_arguments, RetType* result) {
    return engine_.Execute(exec_ctx, entry_arguments, result);
  }

  [[nodiscard]] std::string_view GetIRCode() const { return engine_.GetIRCode(); }

 private:
  ExecEngine engine_;
};

// AthenaPipeline compiles a multi-statement DSL program into a batch of
// entry functions that share one JIT module. Pipelines do not return a
// value through RetType — they write their results via user-supplied
// store functions registered through RegisterStoreCFunc. This matches
// feature-processing / columnar-store scenarios.
//
// For the simpler "one expression, one return value" form, use
// AthenaExpression above.
//
// Thread-safety contract (mirrors BatchExecEngine):
// - Compile() is NOT thread-safe.
// - After a successful Compile(), the Execute() overloads are safe to
//   invoke concurrently from multiple threads, provided each thread uses
//   its own ExecContext.
// - The Execute() overloads that do not take an ExecContext internally
//   construct a fresh one per call (thread-safe but pay per-call
//   allocation cost). Prefer the ExecContext& overloads on hot paths.
// - A single ExecContext must NOT be shared across threads. Recommended
//   pattern for parallel execution: 1 AthenaPipeline + N ExecContexts
//   (one per worker).
class AthenaPipeline {
 public:
  explicit AthenaPipeline(ExecEngineOption option = {}) : engine_(option) {}
  ~AthenaPipeline() = default;
  AthenaPipeline(const AthenaPipeline&) = delete;
  AthenaPipeline& operator=(const AthenaPipeline&) = delete;

  // Compile a list of DSL pipeline statements. The grouper merges
  // statements that share inputs; every group becomes one entry in the
  // underlying BatchExecEngine.
  Status Compile(const std::vector<std::string>& code, const std::unique_ptr<FunctionRegistry>& func_registry);

  // Run every compiled entry in order, forwarding `result` to each entry's
  // user store functions.
  Status Execute(void* entry_arguments, void* result) { return engine_.ExecuteAll(entry_arguments, result); }
  Status Execute(ExecContext& exec_ctx, void* entry_arguments, void* result) {
    return engine_.ExecuteAll(exec_ctx, entry_arguments, result);
  }

  [[nodiscard]] std::string_view GetIRCode() const { return engine_.GetIRCode(); }

 private:
  BatchExecEngine engine_;
};

}  // namespace athena

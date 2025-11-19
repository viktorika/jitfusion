/*
 * @Author: victorika
 * @Date: 2025-04-09 15:43:45
 * @Last Modified by: victorika
 * @Last Modified time: 2025-04-16 17:04:47
 */
#pragma once

#include <memory>
#include "arena.h"
#include "exec_engine.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace athena {

using jitfusion::Arena;
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

class Athena {
 public:
  explicit Athena(ExecEngineOption option = {}) : exec_engine_(option) {}
  ~Athena() = default;
  Athena(const Athena&) = delete;
  Athena& operator=(const Athena&) = delete;

  // Applicable to simple scenarios, the program will not actually use a custom store function to write data. Instead,
  // the result will be returned, similar to expression scenarios.
  // If you need to optimize the memory allocation issue of ExecContext, you can use the function passed to ExecContext.
  Status Compile(const std::string& code, const std::unique_ptr<FunctionRegistry>& func_registry);
  Status Execute(void* entry_arguments, RetType* result);
  Status Execute(ExecContext& exec_ctx, void* entry_arguments, RetType* result);

  // Applicable to complex scenarios where multiple pipelines are computed simultaneously. Each pipeline writes data
  // using a custom function, and results are not returned. This is similar to feature processing scenarios.
  // If you need to optimize the memory allocation issue of ExecContext, you can use the function passed to ExecContext.
  Status Compile(const std::vector<std::string>& code, const std::unique_ptr<FunctionRegistry>& func_registry);
  Status Execute(void* entry_arguments, void* result);
  Status Execute(ExecContext& exec_ctx, void* entry_arguments, void* result);

  [[nodiscard]] std::string_view GetIRCode() const { return exec_engine_.GetIRCode(); }

 private:
  ExecEngine exec_engine_;
};

}  // namespace athena
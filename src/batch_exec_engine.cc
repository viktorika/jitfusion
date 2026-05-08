/*
 * @Author: victorika
 * @Date: 2026-05-07 18:00:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 18:00:00
 */
#include "batch_exec_engine.h"

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "compiled_artifact.h"
#include "exec_node.h"
#include "execute_dispatch.h"
#include "function_registry.h"
#include "jit_core.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/TargetParser/Host.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

BatchExecEngine::BatchExecEngine(ExecEngineOption option) : core_(std::make_unique<JitCore>(option)) {}
BatchExecEngine::~BatchExecEngine() = default;

std::string_view BatchExecEngine::GetIRCode() const { return core_->GetIRCode(); }

Status BatchExecEngine::Compile(const std::vector<std::unique_ptr<ExecNode>>& exec_nodes,
                                const std::unique_ptr<FunctionRegistry>& func_registry) {
  if (exec_nodes.empty()) {
    return Status::ParseError("Compile: exec_nodes is empty");
  }
  entry_func_ptrs_.clear();
  ret_types_.clear();

  std::vector<ExecNode*> nodes;
  nodes.reserve(exec_nodes.size());
  std::vector<std::string> names;
  names.reserve(exec_nodes.size());
  for (size_t i = 0; i < exec_nodes.size(); ++i) {
    nodes.emplace_back(exec_nodes[i].get());
    names.emplace_back(kEntryFunctionName + std::to_string(i));
  }
  return core_->BuildAndLink(nodes, names, func_registry, &entry_func_ptrs_, &ret_types_);
}

Status BatchExecEngine::ExecuteAt(size_t index, void* entry_arguments, RetType* result) {
  if (index >= entry_func_ptrs_.size()) {
    return Status::RuntimeError("ExecuteAt: index out of range");
  }
  ExecContext exec_ctx(core_->GetOption().exec_ctx_arena_alloc_min_chunk_size);
  char* func_ptr = entry_func_ptrs_[index];
  ValueType ret_type = ret_types_[index];
  JF_EXPAND_SWITCH_TYPE(func_ptr, ret_type)
  if (exec_ctx.HasErrors()) {
    return Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  return Status::OK();
}

Status BatchExecEngine::ExecuteAt(size_t index, ExecContext& exec_ctx, void* entry_arguments, RetType* result) {
  if (index >= entry_func_ptrs_.size()) {
    return Status::RuntimeError("ExecuteAt: index out of range");
  }
  char* func_ptr = entry_func_ptrs_[index];
  ValueType ret_type = ret_types_[index];
  JF_EXPAND_SWITCH_TYPE(func_ptr, ret_type)
  Status ret = Status::OK();
  if (exec_ctx.HasErrors()) {
    ret = Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  exec_ctx.Clear();
  return ret;
}

Status BatchExecEngine::ExecuteAt(size_t index, void* entry_arguments, void* result) {
  if (index >= entry_func_ptrs_.size()) {
    return Status::RuntimeError("ExecuteAt: index out of range");
  }
  ExecContext exec_ctx(core_->GetOption().exec_ctx_arena_alloc_min_chunk_size);
  reinterpret_cast<::jitfusion::execute_dispatch::return_void_function_type>(entry_func_ptrs_[index])(
      entry_arguments, &exec_ctx, result);
  if (exec_ctx.HasErrors()) {
    return Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  return Status::OK();
}

Status BatchExecEngine::ExecuteAt(size_t index, ExecContext& exec_ctx, void* entry_arguments, void* result) {
  if (index >= entry_func_ptrs_.size()) {
    return Status::RuntimeError("ExecuteAt: index out of range");
  }
  reinterpret_cast<::jitfusion::execute_dispatch::return_void_function_type>(entry_func_ptrs_[index])(
      entry_arguments, &exec_ctx, result);
  Status ret = Status::OK();
  if (exec_ctx.HasErrors()) {
    ret = Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  exec_ctx.Clear();
  return ret;
}

Status BatchExecEngine::ExecuteAll(void* entry_arguments, std::vector<RetType>* results) {
  results->resize(entry_func_ptrs_.size());
  std::string all_errors;
  for (size_t i = 0; i < entry_func_ptrs_.size(); ++i) {
    Status st = ExecuteAt(i, entry_arguments, &(*results)[i]);
    if (!st.ok()) {
      if (!all_errors.empty()) {
        all_errors += "| ";
      }
      all_errors += st.ToString();
    }
  }
  if (!all_errors.empty()) {
    return Status::RuntimeError(all_errors);
  }
  return Status::OK();
}

Status BatchExecEngine::ExecuteAll(ExecContext& exec_ctx, void* entry_arguments, std::vector<RetType>* results) {
  results->resize(entry_func_ptrs_.size());
  std::string all_errors;
  for (size_t i = 0; i < entry_func_ptrs_.size(); ++i) {
    Status st = ExecuteAt(i, exec_ctx, entry_arguments, &(*results)[i]);
    if (!st.ok()) {
      if (!all_errors.empty()) {
        all_errors += "| ";
      }
      all_errors += st.ToString();
    }
  }
  if (!all_errors.empty()) {
    return Status::RuntimeError(all_errors);
  }
  return Status::OK();
}

Status BatchExecEngine::ExecuteAll(void* entry_arguments, void* results) {
  std::string all_errors;
  for (size_t i = 0; i < entry_func_ptrs_.size(); ++i) {
    Status st = ExecuteAt(i, entry_arguments, results);
    if (!st.ok()) {
      if (!all_errors.empty()) {
        all_errors += "| ";
      }
      all_errors += st.ToString();
    }
  }
  if (!all_errors.empty()) {
    return Status::RuntimeError(all_errors);
  }
  return Status::OK();
}

Status BatchExecEngine::ExecuteAll(ExecContext& exec_ctx, void* entry_arguments, void* results) {
  std::string all_errors;
  for (size_t i = 0; i < entry_func_ptrs_.size(); ++i) {
    Status st = ExecuteAt(i, exec_ctx, entry_arguments, results);
    if (!st.ok()) {
      if (!all_errors.empty()) {
        all_errors += "| ";
      }
      all_errors += st.ToString();
    }
  }
  if (!all_errors.empty()) {
    return Status::RuntimeError(all_errors);
  }
  return Status::OK();
}

Status BatchExecEngine::SaveCompiled(std::string* out) const {
  if (out == nullptr) {
    return Status::InvalidArgument("SaveCompiled: out must not be null");
  }
  if (entry_func_ptrs_.empty()) {
    return Status::RuntimeError("SaveCompiled: nothing compiled yet. Call Compile() first.");
  }
  const std::string_view obj = core_->GetCompiledObjectBytes();
  if (obj.empty()) {
    return Status::RuntimeError(
        "SaveCompiled: no object bytes captured. Set "
        "ExecEngineOption::enable_save_compiled=true BEFORE calling Compile(), "
        "then re-compile. (Artifacts loaded via LoadCompiled also cannot be "
        "re-saved — the loaded bytes are intentionally not retained.)");
  }

  const ExecEngineOption& option = core_->GetOption();
  ArtifactHeader header;
  header.llvm_version = LLVM_VERSION_STRING;
  header.target_triple = llvm::sys::getProcessTriple();
  header.cpu_name = llvm::sys::getHostCPUName().str();
  header.mode = ArtifactMode::kBatch;
  header.fp_math_mode = option.fp_math_mode;
  header.per_entry_ret_types = ret_types_;
  return SerializeArtifact(header, obj, out);
}

Status BatchExecEngine::LoadCompiled(std::string_view bytes, const std::unique_ptr<FunctionRegistry>& func_registry) {
  ArtifactHeader header;
  std::string_view obj_view;
  JF_RETURN_NOT_OK(DeserializeArtifact(bytes, &header, &obj_view));

  if (header.llvm_version != LLVM_VERSION_STRING) {
    return Status::InvalidArgument(
        "compiled artifact: LLVM version mismatch (artifact=", header.llvm_version, ", runtime=", LLVM_VERSION_STRING,
        "). LLVM does not guarantee object-file compatibility across versions; regenerate the artifact.");
  }
  {
    const std::string current_triple = llvm::sys::getProcessTriple();
    if (header.target_triple != current_triple) {
      return Status::InvalidArgument("compiled artifact: target triple mismatch (artifact=", header.target_triple,
                                     ", runtime=", current_triple, ")");
    }
  }
  {
    const std::string current_cpu = llvm::sys::getHostCPUName().str();
    if (header.cpu_name != current_cpu) {
      return Status::InvalidArgument("compiled artifact: host CPU mismatch (artifact=", header.cpu_name,
                                     ", runtime=", current_cpu, "). Regenerate the artifact on this host.");
    }
  }
  if (header.mode != ArtifactMode::kBatch) {
    return Status::InvalidArgument(
        "compiled artifact: mode mismatch — this blob was saved by ExecEngine. "
        "Load it with ExecEngine::LoadCompiled instead.");
  }

  ExecEngineOption new_opt = core_->GetOption();
  new_opt.fp_math_mode = header.fp_math_mode;
  core_ = std::make_unique<JitCore>(new_opt);

  const size_t n = header.per_entry_ret_types.size();
  std::vector<std::string> symbol_names;
  symbol_names.reserve(n);
  for (size_t i = 0; i < n; ++i) {
    symbol_names.emplace_back(kEntryFunctionName + std::to_string(i));
  }
  std::vector<char*> func_ptrs;
  JF_RETURN_NOT_OK(core_->LoadObject(obj_view, symbol_names, func_registry, &func_ptrs));

  entry_func_ptrs_ = std::move(func_ptrs);
  ret_types_ = std::move(header.per_entry_ret_types);
  return Status::OK();
}

}  // namespace jitfusion

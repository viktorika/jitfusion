/*
 * @Author: victorika
 * @Date: 2025-01-15 10:59:33
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 18:00:00
 */
#include "exec_engine.h"

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

ExecEngine::ExecEngine(ExecEngineOption option) : core_(std::make_unique<JitCore>(option)) {}
ExecEngine::~ExecEngine() = default;

std::string_view ExecEngine::GetIRCode() const { return core_->GetIRCode(); }

Status ExecEngine::Compile(const std::unique_ptr<ExecNode>& exec_node,
                           const std::unique_ptr<FunctionRegistry>& func_registry) {
  // Clear previous compilation state up front so that a failed Compile
  // leaves the engine in a cleanly-uncompiled state rather than a half-
  // configured one.
  entry_func_ptr_ = nullptr;
  ret_type_ = ValueType::kUnknown;

  std::vector<ExecNode*> nodes{exec_node.get()};
  std::vector<std::string> names{kEntryFunctionName};
  std::vector<char*> func_ptrs;
  std::vector<ValueType> ret_types;
  JF_RETURN_NOT_OK(core_->BuildAndLink(nodes, names, func_registry, &func_ptrs, &ret_types));

  entry_func_ptr_ = func_ptrs[0];
  ret_type_ = ret_types[0];
  return Status::OK();
}

Status ExecEngine::Execute(void* entry_arguments, RetType* result) {
  ExecContext exec_ctx(core_->GetOption().exec_ctx_arena_alloc_min_chunk_size);
  JF_EXPAND_SWITCH_TYPE(entry_func_ptr_, ret_type_)
  if (exec_ctx.HasErrors()) {
    return Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  return Status::OK();
}

Status ExecEngine::Execute(ExecContext& exec_ctx, void* entry_arguments, RetType* result) {
  JF_EXPAND_SWITCH_TYPE(entry_func_ptr_, ret_type_)
  Status ret = Status::OK();
  if (exec_ctx.HasErrors()) {
    ret = Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  exec_ctx.Clear();
  return ret;
}

Status ExecEngine::Execute(void* entry_arguments, void* result) {
  ExecContext exec_ctx(core_->GetOption().exec_ctx_arena_alloc_min_chunk_size);
  reinterpret_cast<::jitfusion::execute_dispatch::return_void_function_type>(entry_func_ptr_)(entry_arguments,
                                                                                              &exec_ctx, result);
  if (exec_ctx.HasErrors()) {
    return Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  return Status::OK();
}

Status ExecEngine::Execute(ExecContext& exec_ctx, void* entry_arguments, void* result) {
  reinterpret_cast<::jitfusion::execute_dispatch::return_void_function_type>(entry_func_ptr_)(entry_arguments,
                                                                                              &exec_ctx, result);
  Status ret = Status::OK();
  if (exec_ctx.HasErrors()) {
    ret = Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  exec_ctx.Clear();
  return ret;
}

Status ExecEngine::SaveCompiled(std::string* out) const {
  if (out == nullptr) {
    return Status::InvalidArgument("SaveCompiled: out must not be null");
  }
  if (entry_func_ptr_ == nullptr) {
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
  header.mode = ArtifactMode::kSingle;
  header.fp_math_mode = option.fp_math_mode;
  header.per_entry_ret_types = {ret_type_};
  return SerializeArtifact(header, obj, out);
}

Status ExecEngine::LoadCompiled(std::string_view bytes, const std::unique_ptr<FunctionRegistry>& func_registry) {
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
  if (header.mode != ArtifactMode::kSingle) {
    return Status::InvalidArgument(
        "compiled artifact: mode mismatch — this blob was saved by BatchExecEngine. "
        "Load it with BatchExecEngine::LoadCompiled instead.");
  }

  // Rebuild a JitCore that reflects the FP mode actually baked into the
  // artifact (FP mode affects codegen flags, but the object bytes are
  // already codegen'd — we only need the LLJIT to match so that any
  // linker flags stay consistent).
  ExecEngineOption new_opt = core_->GetOption();
  new_opt.fp_math_mode = header.fp_math_mode;
  core_ = std::make_unique<JitCore>(new_opt);

  std::vector<std::string> symbol_names{kEntryFunctionName};
  std::vector<char*> func_ptrs;
  JF_RETURN_NOT_OK(core_->LoadObject(obj_view, symbol_names, func_registry, &func_ptrs));

  entry_func_ptr_ = func_ptrs[0];
  // DeserializeArtifact guarantees per_entry_ret_types is non-empty
  // (num_entries >= 1, and single-mode blobs specifically have exactly
  // one entry). Indexing [0] is therefore always safe here.
  ret_type_ = header.per_entry_ret_types[0];
  return Status::OK();
}

}  // namespace jitfusion

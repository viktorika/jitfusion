/*
 * @Author: victorika
 * @Date: 2025-01-15 10:59:33
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-20 15:12:03
 */
#include "exec_engine.h"
#include <status.h>
#include <iostream>
#include "llvm/ADT/StringMap.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
// #include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "validator.h"

namespace jitfusion {

namespace {

std::once_flag llvm_target_initialized;

std::unique_ptr<llvm::TargetMachine> GetTargetMachine() {
  std::call_once(llvm_target_initialized, []() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetDisassembler();
    llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
  });

  std::string error;
  auto* cpu = LLVMGetHostCPUName();
  auto* triple = LLVMGetDefaultTargetTriple();
  const auto* target = llvm::TargetRegistry::lookupTarget(triple, error);

  llvm::SubtargetFeatures features;
  llvm::StringMap<bool> feature_map = llvm::sys::getHostCPUFeatures();
  for (auto& f : feature_map) {
    features.AddFeature(f.first(), f.second);
  }
  llvm::TargetOptions options;
  auto* target_machine = target->createTargetMachine(triple, cpu, features.getString(), options, std::nullopt,
                                                     std::nullopt, llvm::CodeGenOptLevel::Aggressive, true);

  return std::unique_ptr<llvm::TargetMachine>(target_machine);
};

}  // namespace

Status ExecEngine::Compile(const std::unique_ptr<ExecNode>& exec_node,
                           const std::unique_ptr<FunctionRegistry>& func_registry) {
  // validator
  Validator validator(func_registry);
  RETURN_NOT_OK(validator.Validate(exec_node.get()));

  // codegen
  llvm::LLVMContext context;
  std::unique_ptr<llvm::Module> owner = std::make_unique<llvm::Module>("module", context);
  llvm::Module* module = owner.get();

  // veriry

  return Status::OK();
}

}  // namespace jitfusion
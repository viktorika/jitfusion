/*
 * @Author: victorika
 * @Date: 2025-01-15 10:59:33
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-22 16:52:16
 */
#include "exec_engine.h"
#include <exec_node.h>
#include <status.h>
#include <type.h>
#include <memory>
#include <vector>
#include "codegen/codegen.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/Transforms/IPO/AlwaysInliner.h"
#include "llvm/Transforms/IPO/DeadArgumentElimination.h"
#include "llvm/Transforms/IPO/Inliner.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/DCE.h"
#include "llvm/Transforms/Scalar/EarlyCSE.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/IndVarSimplify.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/Transforms/Scalar/MergedLoadStoreMotion.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/Transforms/Vectorize/LoopVectorize.h"
#include "llvm/Transforms/Vectorize/SLPVectorizer.h"
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
  if (exec_node->GetExecNodeType() != ExecNodeType::kNoOPNode) {
    return Status::InvalidArgument("Root node must be no op node");
  }
  // validator
  Validator validator(func_registry);
  RETURN_NOT_OK(validator.Validate(exec_node.get()));

  // codegen
  llvm::LLVMContext context;
  std::unique_ptr<llvm::Module> owner = std::make_unique<llvm::Module>("module", context);
  llvm::Module* m = owner.get();

  std::vector<llvm::Type*> complex_type_fields = {llvm::Type::getInt64Ty(context), llvm::Type::getInt32Ty(context)};
  llvm::StructType* complex_type = llvm::StructType::create(context, complex_type_fields, "ComplexSruct");
  auto entry_func_callee = m->getOrInsertFunction("entry", llvm::Type::getInt8Ty(context),
                                                  llvm::Type::getInt64Ty(context), llvm::Type::getInt64Ty(context));
  auto* entry_function = llvm::cast<llvm::Function>(entry_func_callee.getCallee());
  llvm::BasicBlock* entry_bb = llvm::BasicBlock::Create(context, "entryBB", entry_function);
  llvm::IRBuilder<> builder(entry_bb);
  std::unique_ptr<IRCodeGenContext> codegen_ctx = std::make_unique<IRCodeGenContext>(
      context, *m, builder, entry_bb, entry_function, complex_type, func_registry_, const_value_arena_);
  CodeGen codegen(*codegen_ctx);
  llvm::Value* ret_value;
  RETURN_NOT_OK(codegen.GetValue(exec_node.get(), &ret_value));
  builder.CreateRet(ret_value);

  // verify
  std::string error_info;
  llvm::raw_string_ostream error_stream(error_info);
  if (llvm::verifyModule(*m, &error_stream)) {
    return Status::RuntimeError("Module verification failed: " + error_info);
  }

  // optimize
  auto machine = GetTargetMachine();

  llvm::PassBuilder pass_builder;
  llvm::ModulePassManager mpm;
  llvm::LoopAnalysisManager lam;
  llvm::FunctionAnalysisManager fam;
  llvm::CGSCCAnalysisManager cgam;
  llvm::ModuleAnalysisManager mam;

  pass_builder.registerModuleAnalyses(mam);
  pass_builder.registerCGSCCAnalyses(cgam);
  pass_builder.registerFunctionAnalyses(fam);
  pass_builder.registerLoopAnalyses(lam);
  pass_builder.crossRegisterProxies(lam, fam, cgam, mam);

  llvm::OptimizationLevel opt_level = llvm::OptimizationLevel::O3;
  mpm = pass_builder.buildPerModuleDefaultPipeline(opt_level);
  mpm.addPass(llvm::createFunctionToLoopPassAdaptor(llvm::LoopVectorizePass()));
  mpm.addPass(llvm::SLPVectorizerPass());
  mpm.addPass(llvm::SimplifyCFGPass());
  mpm.addPass(llvm::EarlyCSEPass());
  mpm.addPass(llvm::GVNPass());
  mpm.addPass(llvm::ReassociatePass());
  mpm.addPass(llvm::IndVarSimplifyPass());
  mpm.addPass(llvm::MergedLoadStoreMotionPass());
  mpm.addPass(llvm::AlwaysInlinerPass());
  mpm.addPass(llvm::InlinerPass());
  mpm.addPass(llvm::DeadArgumentEliminationPass());

  mpm.run(*m, mam);

  // Now we create the JIT.
  engine_ = llvm::EngineBuilder(std::move(owner)).setEngineKind(llvm::EngineKind::JIT).create();
  func_registry_->MappingToLLVM(engine_);
  engine_->finalizeObject();

  entry_func_ptr_ = reinterpret_cast<entry_func_type>(engine_->getFunctionAddress("entry"));
  return Status::OK();
}

Status ExecEngine::Execute(void* entry_arguments) {
  if (0 == entry_func_ptr_) {
    return Status::RuntimeError("Please compile before execute");
  }
  ExecContext exec_ctx;
  (entry_func_ptr_)(reinterpret_cast<int64_t>(entry_arguments), reinterpret_cast<int64_t>(&exec_ctx));
  return Status::OK();
}

}  // namespace jitfusion
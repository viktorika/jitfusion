/*
 * @Author: victorika
 * @Date: 2025-01-15 10:59:33
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-24 11:11:57
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

llvm::TargetMachine* GetTargetMachine() {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  llvm::InitializeNativeTargetDisassembler();
  llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

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

  return target_machine;
};

using return_u8_function_type = uint8_t (*)(int64_t, int64_t);
using return_u16_function_type = uint16_t (*)(int64_t, int64_t);
using return_u32_function_type = uint32_t (*)(int64_t, int64_t);
using return_u64_function_type = uint64_t (*)(int64_t, int64_t);
using return_i8_function_type = int8_t (*)(int64_t, int64_t);
using return_i16_function_type = int16_t (*)(int64_t, int64_t);
using return_i32_function_type = int32_t (*)(int64_t, int64_t);
using return_i64_function_type = int64_t (*)(int64_t, int64_t);
using return_f32_function_type = float (*)(int64_t, int64_t);
using return_f64_function_type = double (*)(int64_t, int64_t);
using return_string_function_type = LLVMComplexStruct (*)(int64_t, int64_t);
using return_list_function_type = LLVMComplexStruct (*)(int64_t, int64_t);

using return_u8_multigroup_function_type = uint8_t (*)(int64_t, int64_t);
using return_u16_multigroup_function_type = uint16_t (*)(int64_t, int64_t);
using return_u32_multigroup_function_type = uint32_t (*)(int64_t, int64_t);
using return_u64_multigroup_function_type = uint64_t (*)(int64_t, int64_t);
using return_i8_multigroup_function_type = int8_t (*)(int64_t, int64_t);
using return_i16_multigroup_function_type = int16_t (*)(int64_t, int64_t);
using return_i32_multigroup_function_type = int32_t (*)(int64_t, int64_t);
using return_i64_multigroup_function_type = int64_t (*)(int64_t, int64_t);
using return_f32_multigroup_function_type = float (*)(int64_t, int64_t);
using return_f64_multigroup_function_type = double (*)(int64_t, int64_t);
using return_string_multigroup_function_type = LLVMComplexStruct (*)(int64_t, int64_t);
using return_list_multigroup_function_type = LLVMComplexStruct (*)(int64_t, int64_t);

}  // namespace

Status ExecEngine::Compile(const std::unique_ptr<ExecNode>& exec_node,
                           const std::unique_ptr<FunctionRegistry>& func_registry) {
  // if (exec_node->GetExecNodeType() != ExecNodeType::kNoOPNode) {
  //   return Status::InvalidArgument("Root node must be no op node");
  // }
  // validator
  Validator validator(func_registry);
  RETURN_NOT_OK(validator.Validate(exec_node.get()));
  ret_type_ = exec_node->GetReturnType();

  // codegen
  llvm::LLVMContext context;
  std::unique_ptr<llvm::Module> owner = std::make_unique<llvm::Module>("module", context);
  llvm::Module* m = owner.get();

  std::vector<llvm::Type*> complex_type_fields = {llvm::Type::getInt64Ty(context), llvm::Type::getInt32Ty(context)};
  llvm::StructType* complex_type = llvm::StructType::create(context, complex_type_fields, "ComplexSruct");

  llvm::FunctionCallee entry_func_callee;
  switch (ret_type_) {
    case ValueType::kU8:
    case ValueType::kI8:
      entry_func_callee = m->getOrInsertFunction("entry", llvm::Type::getInt8Ty(context),
                                                 llvm::Type::getInt64Ty(context), llvm::Type::getInt64Ty(context));
      break;
    case ValueType::kU16:
    case ValueType::kI16:
      entry_func_callee = m->getOrInsertFunction("entry", llvm::Type::getInt16Ty(context),
                                                 llvm::Type::getInt64Ty(context), llvm::Type::getInt64Ty(context));
      break;
    case ValueType::kU32:
    case ValueType::kI32:
      entry_func_callee = m->getOrInsertFunction("entry", llvm::Type::getInt32Ty(context),
                                                 llvm::Type::getInt64Ty(context), llvm::Type::getInt64Ty(context));
      break;
    case ValueType::kU64:
    case ValueType::kI64:
      entry_func_callee = m->getOrInsertFunction("entry", llvm::Type::getInt64Ty(context),
                                                 llvm::Type::getInt64Ty(context), llvm::Type::getInt64Ty(context));
      break;
    case ValueType::kF32:
      entry_func_callee = m->getOrInsertFunction("entry", llvm::Type::getFloatTy(context),
                                                 llvm::Type::getInt64Ty(context), llvm::Type::getInt64Ty(context));
      break;
    case ValueType::kF64:
      entry_func_callee = m->getOrInsertFunction("entry", llvm::Type::getDoubleTy(context),
                                                 llvm::Type::getInt64Ty(context), llvm::Type::getInt64Ty(context));
      break;
    case ValueType::kString:
    case ValueType::kU8List:
    case ValueType::kU16List:
    case ValueType::kU32List:
    case ValueType::kU64List:
    case ValueType::kI8List:
    case ValueType::kI16List:
    case ValueType::kI32List:
    case ValueType::kI64List:
    case ValueType::kF32List:
    case ValueType::kF64List:
    case ValueType::kStringList:
      entry_func_callee = m->getOrInsertFunction("entry", complex_type, llvm::Type::getInt64Ty(context),
                                                 llvm::Type::getInt64Ty(context));
      break;
    default:
      return Status::ParseError("Unknown return type: ", TypeHelper::TypeToString(ret_type_));
  }
  // auto entry_func_callee = m->getOrInsertFunction("entry", llvm::Type::getInt8Ty(context),
  //                                                 llvm::Type::getInt64Ty(context), llvm::Type::getInt64Ty(context));

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
  static auto* machine = GetTargetMachine();

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

  auto tti = machine->getTargetIRAnalysis();
  fam.registerPass([&] { return tti; });

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

  entry_func_ptr_ = engine_->getFunctionAddress("entry");
  return Status::OK();
}

Status ExecEngine::Execute(void* entry_arguments, RetType* result) {
  ExecContext exec_ctx;
  switch (ret_type_) {
    case ValueType::kU8:
      *result = reinterpret_cast<return_u8_function_type>(entry_func_ptr_)(reinterpret_cast<int64_t>(entry_arguments),
                                                                           reinterpret_cast<int64_t>(&exec_ctx));
      break;
    case ValueType::kI8:
      *result = reinterpret_cast<return_i8_function_type>(entry_func_ptr_)(reinterpret_cast<int64_t>(entry_arguments),
                                                                           reinterpret_cast<int64_t>(&exec_ctx));
      break;
    case ValueType::kU16:
      *result = reinterpret_cast<return_u16_function_type>(entry_func_ptr_)(reinterpret_cast<int64_t>(entry_arguments),
                                                                            reinterpret_cast<int64_t>(&exec_ctx));
      break;
    case ValueType::kI16:
      *result = reinterpret_cast<return_i16_function_type>(entry_func_ptr_)(reinterpret_cast<int64_t>(entry_arguments),
                                                                            reinterpret_cast<int64_t>(&exec_ctx));
      break;
    case ValueType::kU32:
      *result = reinterpret_cast<return_u32_function_type>(entry_func_ptr_)(reinterpret_cast<int64_t>(entry_arguments),
                                                                            reinterpret_cast<int64_t>(&exec_ctx));
      break;
    case ValueType::kI32:
      *result = reinterpret_cast<return_i32_function_type>(entry_func_ptr_)(reinterpret_cast<int64_t>(entry_arguments),
                                                                            reinterpret_cast<int64_t>(&exec_ctx));
      break;
    case ValueType::kU64:
      *result = reinterpret_cast<return_u64_function_type>(entry_func_ptr_)(reinterpret_cast<int64_t>(entry_arguments),
                                                                            reinterpret_cast<int64_t>(&exec_ctx));
      break;
    case ValueType::kI64:
      *result = reinterpret_cast<return_i64_function_type>(entry_func_ptr_)(reinterpret_cast<int64_t>(entry_arguments),
                                                                            reinterpret_cast<int64_t>(&exec_ctx));
      break;
    case ValueType::kF32:
      *result = reinterpret_cast<return_f32_function_type>(entry_func_ptr_)(reinterpret_cast<int64_t>(entry_arguments),
                                                                            reinterpret_cast<int64_t>(&exec_ctx));
      break;
    case ValueType::kF64:
      *result = reinterpret_cast<return_f64_function_type>(entry_func_ptr_)(reinterpret_cast<int64_t>(entry_arguments),
                                                                            reinterpret_cast<int64_t>(&exec_ctx));
      break;
    case ValueType::kString: {
      LLVMComplexStruct string = reinterpret_cast<return_string_function_type>(entry_func_ptr_)(
          reinterpret_cast<int64_t>(entry_arguments), reinterpret_cast<int64_t>(&exec_ctx));
      std::string res(reinterpret_cast<char*>(string.data), string.len);
      *result = std::move(res);
    } break;
    case ValueType::kU8List: {
      LLVMComplexStruct list = reinterpret_cast<return_list_function_type>(entry_func_ptr_)(
          reinterpret_cast<int64_t>(entry_arguments), reinterpret_cast<int64_t>(&exec_ctx));
      std::vector<uint8_t> res;
      res.resize(list.len);
      memcpy(res.data(), reinterpret_cast<uint8_t*>(list.data), list.len * sizeof(uint8_t));
      *result = std::move(res);
    } break;
    case ValueType::kU16List: {
      LLVMComplexStruct list = reinterpret_cast<return_list_function_type>(entry_func_ptr_)(
          reinterpret_cast<int64_t>(entry_arguments), reinterpret_cast<int64_t>(&exec_ctx));
      std::vector<uint16_t> res;
      res.resize(list.len);
      memcpy(res.data(), reinterpret_cast<uint16_t*>(list.data), list.len * sizeof(uint16_t));
      *result = std::move(res);
    } break;
    case ValueType::kU32List: {
      LLVMComplexStruct list = reinterpret_cast<return_list_function_type>(entry_func_ptr_)(
          reinterpret_cast<int64_t>(entry_arguments), reinterpret_cast<int64_t>(&exec_ctx));
      std::vector<uint32_t> res;
      res.resize(list.len);
      memcpy(res.data(), reinterpret_cast<uint32_t*>(list.data), list.len * sizeof(uint32_t));
      *result = std::move(res);
    } break;
    case ValueType::kU64List: {
      LLVMComplexStruct list = reinterpret_cast<return_list_function_type>(entry_func_ptr_)(
          reinterpret_cast<int64_t>(entry_arguments), reinterpret_cast<int64_t>(&exec_ctx));
      std::vector<uint64_t> res;
      res.resize(list.len);
      memcpy(res.data(), reinterpret_cast<uint64_t*>(list.data), list.len * sizeof(uint64_t));
      *result = std::move(res);
    } break;
    case ValueType::kI8List: {
      LLVMComplexStruct list = reinterpret_cast<return_list_function_type>(entry_func_ptr_)(
          reinterpret_cast<int64_t>(entry_arguments), reinterpret_cast<int64_t>(&exec_ctx));
      std::vector<int8_t> res;
      res.resize(list.len);
      memcpy(res.data(), reinterpret_cast<int8_t*>(list.data), list.len * sizeof(int8_t));
      *result = std::move(res);
    } break;
    case ValueType::kI16List: {
      LLVMComplexStruct list = reinterpret_cast<return_list_function_type>(entry_func_ptr_)(
          reinterpret_cast<int64_t>(entry_arguments), reinterpret_cast<int64_t>(&exec_ctx));
      std::vector<int16_t> res;
      res.resize(list.len);
      memcpy(res.data(), reinterpret_cast<int16_t*>(list.data), list.len * sizeof(int16_t));
      *result = std::move(res);
    } break;
    case ValueType::kI32List: {
      LLVMComplexStruct list = reinterpret_cast<return_list_function_type>(entry_func_ptr_)(
          reinterpret_cast<int64_t>(entry_arguments), reinterpret_cast<int64_t>(&exec_ctx));
      std::vector<int32_t> res;
      res.resize(list.len);
      memcpy(res.data(), reinterpret_cast<int32_t*>(list.data), list.len * sizeof(int32_t));
      *result = std::move(res);
    } break;
    case ValueType::kI64List: {
      LLVMComplexStruct list = reinterpret_cast<return_list_function_type>(entry_func_ptr_)(
          reinterpret_cast<int64_t>(entry_arguments), reinterpret_cast<int64_t>(&exec_ctx));
      std::vector<int64_t> res;
      res.resize(list.len);
      memcpy(res.data(), reinterpret_cast<int64_t*>(list.data), list.len * sizeof(int64_t));
      *result = std::move(res);
    } break;
    case ValueType::kF32List: {
      LLVMComplexStruct list = reinterpret_cast<return_list_function_type>(entry_func_ptr_)(
          reinterpret_cast<int64_t>(entry_arguments), reinterpret_cast<int64_t>(&exec_ctx));
      std::vector<float> res;
      res.resize(list.len);
      memcpy(res.data(), reinterpret_cast<float*>(list.data), list.len * sizeof(float));
      *result = std::move(res);
    } break;
    case ValueType::kF64List: {
      LLVMComplexStruct list = reinterpret_cast<return_list_function_type>(entry_func_ptr_)(
          reinterpret_cast<int64_t>(entry_arguments), reinterpret_cast<int64_t>(&exec_ctx));
      std::vector<double> res;
      res.resize(list.len);
      memcpy(res.data(), reinterpret_cast<double*>(list.data), list.len * sizeof(double));
      *result = std::move(res);
    } break;
    case ValueType::kStringList: {
      LLVMComplexStruct list = reinterpret_cast<return_list_function_type>(entry_func_ptr_)(
          reinterpret_cast<int64_t>(entry_arguments), reinterpret_cast<int64_t>(&exec_ctx));
      std::vector<std::string> res;
      res.resize(list.len);
      for (int i = 0; i < list.len; ++i) {
        res[i] = std::string(reinterpret_cast<char*>(reinterpret_cast<LLVMComplexStruct*>(list.data)[i].data),
                             reinterpret_cast<LLVMComplexStruct*>(list.data)[i].len);
      }
      *result = std::move(res);
    } break;
    default:
      return Status::RuntimeError("Unsupported return type");
  }

  return Status::OK();
}

}  // namespace jitfusion
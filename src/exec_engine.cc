/*
 * @Author: victorika
 * @Date: 2025-01-15 10:59:33
 * @Last Modified by: victorika
 * @Last Modified time: 2025-04-16 16:49:31
 */
#include "exec_engine.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "codegen/codegen.h"
#include "exec_node.h"
#include "function/function_init.h"
#include "function_registry.h"
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
#include "llvm/Support/Casting.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/Transforms/IPO/AlwaysInliner.h"
#include "llvm/Transforms/IPO/DeadArgumentElimination.h"
#include "llvm/Transforms/IPO/Inliner.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/CallSiteSplitting.h"
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
#include "status.h"
#include "type.h"
#include "validator.h"

namespace jitfusion {

namespace {

llvm::TargetMachine* GetTargetMachine() {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  llvm::InitializeNativeTargetDisassembler();
  llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

  llvm::StringRef cpu = llvm::sys::getHostCPUName();

  llvm::SubtargetFeatures features;
  auto host_features = llvm::sys::getHostCPUFeatures();
  for (auto& f : host_features) {
    features.AddFeature(f.first(), f.second);
  }

  std::string error;
  auto triple = llvm::sys::getDefaultTargetTriple();
  const auto* target = llvm::TargetRegistry::lookupTarget(triple, error);

  llvm::TargetOptions options;
  options.AllowFPOpFusion = llvm::FPOpFusion::Fast;
  options.UnsafeFPMath = true;
  auto* target_machine = target->createTargetMachine(triple, cpu, features.getString(), options, std::nullopt,
                                                     std::nullopt, llvm::CodeGenOptLevel::Aggressive, true);
  return target_machine;
};

LLVMStructType CreateLLVMStructType(llvm::LLVMContext& context) {
  LLVMStructType llvm_struct_type;
  llvm_struct_type.string_type = llvm::StructType::create(
      context, {llvm::Type::getInt8Ty(context)->getPointerTo(), llvm::Type::getInt32Ty(context)}, "StringStruct");
  llvm_struct_type.u8list_type = llvm::StructType::create(
      context, {llvm::Type::getInt8Ty(context)->getPointerTo(), llvm::Type::getInt32Ty(context)}, "U8ListStruct");
  llvm_struct_type.u16list_type = llvm::StructType::create(
      context, {llvm::Type::getInt16Ty(context)->getPointerTo(), llvm::Type::getInt32Ty(context)}, "U16ListStruct");
  llvm_struct_type.u32list_type = llvm::StructType::create(
      context, {llvm::Type::getInt32Ty(context)->getPointerTo(), llvm::Type::getInt32Ty(context)}, "U32ListStruct");
  llvm_struct_type.u64list_type = llvm::StructType::create(
      context, {llvm::Type::getInt64Ty(context)->getPointerTo(), llvm::Type::getInt32Ty(context)}, "U64ListStruct");
  llvm_struct_type.i8list_type = llvm::StructType::create(
      context, {llvm::Type::getInt8Ty(context)->getPointerTo(), llvm::Type::getInt32Ty(context)}, "I8ListStruct");
  llvm_struct_type.i16list_type = llvm::StructType::create(
      context, {llvm::Type::getInt16Ty(context)->getPointerTo(), llvm::Type::getInt32Ty(context)}, "I16ListStruct");
  llvm_struct_type.i32list_type = llvm::StructType::create(
      context, {llvm::Type::getInt32Ty(context)->getPointerTo(), llvm::Type::getInt32Ty(context)}, "I32ListStruct");
  llvm_struct_type.i64list_type = llvm::StructType::create(
      context, {llvm::Type::getInt64Ty(context)->getPointerTo(), llvm::Type::getInt32Ty(context)}, "I64ListStruct");
  llvm_struct_type.f32list_type = llvm::StructType::create(
      context, {llvm::Type::getFloatTy(context)->getPointerTo(), llvm::Type::getInt32Ty(context)}, "F32ListStruct");
  llvm_struct_type.f64list_type = llvm::StructType::create(
      context, {llvm::Type::getDoubleTy(context)->getPointerTo(), llvm::Type::getInt32Ty(context)}, "F64ListStruct");
  llvm_struct_type.stringlist_type = llvm::StructType::create(
      context, {llvm_struct_type.string_type->getPointerTo(), llvm::Type::getInt32Ty(context)}, "StringListStruct");
  return llvm_struct_type;
}

Status GetEntryFunctionCallee(llvm::LLVMContext& context, std::unique_ptr<llvm::Module>& m,
                              LLVMStructType llvm_struct_type, ValueType ret_type,
                              llvm::FunctionCallee* entry_func_callee) {
  switch (ret_type) {
    case ValueType::kU8:
    case ValueType::kI8: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm::Type::getInt8Ty(context), llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU16:
    case ValueType::kI16: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm::Type::getInt16Ty(context), llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU32:
    case ValueType::kI32: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm::Type::getInt32Ty(context), llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU64:
    case ValueType::kI64: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm::Type::getInt64Ty(context), llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kF32: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm::Type::getFloatTy(context), llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kF64: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm::Type::getDoubleTy(context), llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kString: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm_struct_type.string_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU8List: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm_struct_type.u8list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU16List: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm_struct_type.u16list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU32List: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm_struct_type.u32list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU64List: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm_struct_type.u64list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kI8List: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm_struct_type.i8list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kI16List: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm_struct_type.i16list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kI32List: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm_struct_type.i32list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kI64List: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm_struct_type.i64list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kF32List: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm_struct_type.f32list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kF64List: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm_struct_type.f64list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kStringList: {
      *entry_func_callee = m->getOrInsertFunction(
          "entry", llvm_struct_type.stringlist_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    default:
      return Status::ParseError("Unknown return type: ", TypeHelper::TypeToString(ret_type));
  }
  return Status::OK();
}

using return_u8_function_type = uint8_t (*)(void*, void*, void*);
using return_u16_function_type = uint16_t (*)(void*, void*, void*);
using return_u32_function_type = uint32_t (*)(void*, void*, void*);
using return_u64_function_type = uint64_t (*)(void*, void*, void*);
using return_i8_function_type = int8_t (*)(void*, void*, void*);
using return_i16_function_type = int16_t (*)(void*, void*, void*);
using return_i32_function_type = int32_t (*)(void*, void*, void*);
using return_i64_function_type = int64_t (*)(void*, void*, void*);
using return_f32_function_type = float (*)(void*, void*, void*);
using return_f64_function_type = double (*)(void*, void*, void*);
using return_string_function_type = StringStruct (*)(void*, void*, void*);
using return_u8list_function_type = U8ListStruct (*)(void*, void*, void*);
using return_u16list_function_type = U16ListStruct (*)(void*, void*, void*);
using return_u32list_function_type = U32ListStruct (*)(void*, void*, void*);
using return_u64list_function_type = U64ListStruct (*)(void*, void*, void*);
using return_i8list_function_type = I8ListStruct (*)(void*, void*, void*);
using return_i16list_function_type = I16ListStruct (*)(void*, void*, void*);
using return_i32list_function_type = I32ListStruct (*)(void*, void*, void*);
using return_i64list_function_type = I64ListStruct (*)(void*, void*, void*);
using return_f32list_function_type = F32ListStruct (*)(void*, void*, void*);
using return_f64list_function_type = F64ListStruct (*)(void*, void*, void*);
using return_stringlist_function_type = StringListStruct (*)(void*, void*, void*);

}  // namespace

ExecEngine::ExecEngine(ExecEngineOption option)
    : const_value_arena_(option.const_value_arena_alloc_min_chunk_size), option_(option) {}
ExecEngine::~ExecEngine() { delete engine_; }

Status ExecEngine::Compile(const std::unique_ptr<ExecNode>& exec_node,
                           const std::unique_ptr<FunctionRegistry>& func_registry) {
  delete engine_;
  engine_ = nullptr;
  // validator
  Validator validator(func_registry);
  JF_RETURN_NOT_OK(validator.Validate(exec_node.get()));
  ret_type_ = exec_node->GetReturnType();

  // codegen
  std::unique_ptr<llvm::Module> owner = std::make_unique<llvm::Module>("module", llvm_context_);
  llvm::Module* m = owner.get();

  LLVMStructType llvm_struct_type = CreateLLVMStructType(llvm_context_);

  llvm::FunctionCallee entry_func_callee;
  JF_RETURN_NOT_OK(GetEntryFunctionCallee(llvm_context_, owner, llvm_struct_type, ret_type_, &entry_func_callee));
  if (auto* entry_function = llvm::dyn_cast<llvm::Function>(entry_func_callee.getCallee())) {
    entry_function->addAttributeAtIndex(1, llvm::Attribute::get(llvm_context_, llvm::Attribute::NoAlias));
    entry_function->addAttributeAtIndex(1, llvm::Attribute::get(llvm_context_, llvm::Attribute::ReadOnly));
    entry_function->addAttributeAtIndex(2, llvm::Attribute::get(llvm_context_, llvm::Attribute::NoAlias));
    entry_function->addAttributeAtIndex(3, llvm::Attribute::get(llvm_context_, llvm::Attribute::NoAlias));
  }

  auto* entry_function = llvm::cast<llvm::Function>(entry_func_callee.getCallee());
  llvm::BasicBlock* entry_bb = llvm::BasicBlock::Create(llvm_context_, "entryBB", entry_function);
  llvm::IRBuilder<> builder(entry_bb);
  std::unique_ptr<IRCodeGenContext> codegen_ctx = std::make_unique<IRCodeGenContext>(
      llvm_context_, *m, builder, entry_bb, entry_function, llvm_struct_type, func_registry, const_value_arena_);

  CodeGen codegen(*codegen_ctx);

  llvm::Value* ret_value;
  JF_RETURN_NOT_OK(codegen.GetValue(exec_node.get(), &ret_value));
  builder.CreateRet(ret_value);

  // verify
  std::string error_info;
  llvm::raw_string_ostream error_stream(error_info);
  if (llvm::verifyModule(*m, &error_stream)) {
    // m->print(llvm::errs(), nullptr);
    return Status::RuntimeError("Module verification failed: ", error_info);
  }
  // optimize
  static auto* machine = GetTargetMachine();
  llvm::PassBuilder pb(machine);

  llvm::LoopAnalysisManager lam;
  llvm::FunctionAnalysisManager fam;
  llvm::CGSCCAnalysisManager cgam;
  llvm::ModuleAnalysisManager mam;
  llvm::MachineFunctionAnalysisManager mfm;

  fam.registerPass([] { return machine->getTargetIRAnalysis(); });

  // Register all the basic analyses with the managers.
  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);
  pb.registerMachineFunctionAnalyses(mfm);

  pb.crossRegisterProxies(lam, fam, cgam, mam, &mfm);

  // Now we create the JIT.
  engine_ = llvm::EngineBuilder(std::move(owner)).setEngineKind(llvm::EngineKind::JIT).create();
  func_registry->MappingToLLVM(engine_, m);

  // debug
  // m->print(llvm::errs(), nullptr);

  auto mpm = pb.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O3);
  mpm.addPass(llvm::createModuleToFunctionPassAdaptor(llvm::CallSiteSplittingPass()));
  mpm.addPass(llvm::createModuleToFunctionPassAdaptor(llvm::SLPVectorizerPass()));
  mpm.addPass(llvm::createModuleToFunctionPassAdaptor(llvm::VerifierPass()));
  mpm.run(*m, mam);
  engine_->finalizeObject();

  // m->print(llvm::errs(), nullptr);

  uintptr_t entry_func_offset = engine_->getFunctionAddress("entry");
  entry_func_ptr_ = nullptr;
  entry_func_ptr_ += entry_func_offset;
  return Status::OK();
}

#define EXPAND_SWITCH_TYPE                                                                                         \
  switch (ret_type_) {                                                                                             \
    case ValueType::kU8: {                                                                                         \
      *result = reinterpret_cast<return_u8_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);   \
    } break;                                                                                                       \
    case ValueType::kI8: {                                                                                         \
      *result = reinterpret_cast<return_i8_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);   \
    } break;                                                                                                       \
    case ValueType::kU16: {                                                                                        \
      *result = reinterpret_cast<return_u16_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                       \
    case ValueType::kI16: {                                                                                        \
      *result = reinterpret_cast<return_i16_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                       \
    case ValueType::kU32: {                                                                                        \
      *result = reinterpret_cast<return_u32_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                       \
    case ValueType::kI32: {                                                                                        \
      *result = reinterpret_cast<return_i32_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                       \
    case ValueType::kU64: {                                                                                        \
      *result = reinterpret_cast<return_u64_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                       \
    case ValueType::kI64: {                                                                                        \
      *result = reinterpret_cast<return_i64_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                       \
    case ValueType::kF32: {                                                                                        \
      *result = reinterpret_cast<return_f32_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                       \
    case ValueType::kF64: {                                                                                        \
      *result = reinterpret_cast<return_f64_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                       \
    case ValueType::kString: {                                                                                     \
      StringStruct string =                                                                                        \
          reinterpret_cast<return_string_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);     \
      std::string res(string.data, string.len);                                                                    \
      *result = std::move(res);                                                                                    \
    } break;                                                                                                       \
    case ValueType::kU8List: {                                                                                     \
      U8ListStruct list =                                                                                          \
          reinterpret_cast<return_u8list_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);     \
      std::vector<uint8_t> res;                                                                                    \
      res.resize(list.len);                                                                                        \
      memcpy(res.data(), list.data, list.len * sizeof(uint8_t));                                                   \
      *result = std::move(res);                                                                                    \
    } break;                                                                                                       \
    case ValueType::kU16List: {                                                                                    \
      U16ListStruct list =                                                                                         \
          reinterpret_cast<return_u16list_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<uint16_t> res;                                                                                   \
      res.resize(list.len);                                                                                        \
      memcpy(res.data(), list.data, list.len * sizeof(uint16_t));                                                  \
      *result = std::move(res);                                                                                    \
    } break;                                                                                                       \
    case ValueType::kU32List: {                                                                                    \
      U32ListStruct list =                                                                                         \
          reinterpret_cast<return_u32list_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<uint32_t> res;                                                                                   \
      res.resize(list.len);                                                                                        \
      memcpy(res.data(), list.data, list.len * sizeof(uint32_t));                                                  \
      *result = std::move(res);                                                                                    \
    } break;                                                                                                       \
    case ValueType::kU64List: {                                                                                    \
      U64ListStruct list =                                                                                         \
          reinterpret_cast<return_u64list_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<uint64_t> res;                                                                                   \
      res.resize(list.len);                                                                                        \
      memcpy(res.data(), list.data, list.len * sizeof(uint64_t));                                                  \
      *result = std::move(res);                                                                                    \
    } break;                                                                                                       \
    case ValueType::kI8List: {                                                                                     \
      I8ListStruct list =                                                                                          \
          reinterpret_cast<return_i8list_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);     \
      std::vector<int8_t> res;                                                                                     \
      res.resize(list.len);                                                                                        \
      memcpy(res.data(), list.data, list.len * sizeof(int8_t));                                                    \
      *result = std::move(res);                                                                                    \
    } break;                                                                                                       \
    case ValueType::kI16List: {                                                                                    \
      I16ListStruct list =                                                                                         \
          reinterpret_cast<return_i16list_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<int16_t> res;                                                                                    \
      res.resize(list.len);                                                                                        \
      memcpy(res.data(), list.data, list.len * sizeof(int16_t));                                                   \
      *result = std::move(res);                                                                                    \
    } break;                                                                                                       \
    case ValueType::kI32List: {                                                                                    \
      I32ListStruct list =                                                                                         \
          reinterpret_cast<return_i32list_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<int32_t> res;                                                                                    \
      res.resize(list.len);                                                                                        \
      memcpy(res.data(), list.data, list.len * sizeof(int32_t));                                                   \
      *result = std::move(res);                                                                                    \
    } break;                                                                                                       \
    case ValueType::kI64List: {                                                                                    \
      I64ListStruct list =                                                                                         \
          reinterpret_cast<return_i64list_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<int64_t> res;                                                                                    \
      res.resize(list.len);                                                                                        \
      memcpy(res.data(), list.data, list.len * sizeof(int64_t));                                                   \
      *result = std::move(res);                                                                                    \
    } break;                                                                                                       \
    case ValueType::kF32List: {                                                                                    \
      F32ListStruct list =                                                                                         \
          reinterpret_cast<return_f32list_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<float> res;                                                                                      \
      res.resize(list.len);                                                                                        \
      memcpy(res.data(), list.data, list.len * sizeof(float));                                                     \
      *result = std::move(res);                                                                                    \
    } break;                                                                                                       \
    case ValueType::kF64List: {                                                                                    \
      F64ListStruct list =                                                                                         \
          reinterpret_cast<return_f64list_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<double> res;                                                                                     \
      res.resize(list.len);                                                                                        \
      memcpy(res.data(), list.data, list.len * sizeof(double));                                                    \
      *result = std::move(res);                                                                                    \
    } break;                                                                                                       \
    case ValueType::kStringList: {                                                                                 \
      StringListStruct list =                                                                                      \
          reinterpret_cast<return_stringlist_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, nullptr); \
      std::vector<std::string> res;                                                                                \
      res.resize(list.len);                                                                                        \
      for (uint32_t i = 0; i < list.len; ++i) {                                                                    \
        res[i] = std::string(list.data[i].data, list.data[i].len);                                                 \
      }                                                                                                            \
      *result = std::move(res);                                                                                    \
    } break;                                                                                                       \
    default:                                                                                                       \
      return Status::RuntimeError("Unsupported return type");                                                      \
  }

Status ExecEngine::Execute(void* entry_arguments, RetType* result) {
  ExecContext exec_ctx(option_.exec_ctx_arena_alloc_min_chunk_size);
  EXPAND_SWITCH_TYPE
  return Status::OK();
}

Status ExecEngine::Execute(ExecContext& exec_ctx, void* entry_arguments, RetType* result) {
  EXPAND_SWITCH_TYPE
  exec_ctx.Clear();
  return Status::OK();
}

Status ExecEngine::Execute(void* entry_arguments, void* result) {
  ExecContext exec_ctx(option_.exec_ctx_arena_alloc_min_chunk_size);
  reinterpret_cast<return_i8_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, result);
  return Status::OK();
}

Status ExecEngine::Execute(ExecContext& exec_ctx, void* entry_arguments, void* result) {
  reinterpret_cast<return_i8_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, result);
  exec_ctx.Clear();
  return Status::OK();
}

}  // namespace jitfusion
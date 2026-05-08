/*
 * @Author: victorika
 * @Date: 2026-05-07 18:00:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 18:00:00
 */
#include "jit_core.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "codegen/codegen.h"
#include "function/function_init.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/ObjectTransformLayer.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "validator.h"

namespace jitfusion {

const std::string kEntryFunctionName = "entry";

namespace {

struct LLVMInit {
  LLVMInit() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetDisassembler();
    llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
  }
};

LLVMStructType CreateLLVMStructType(llvm::LLVMContext& context) {
  LLVMStructType llvm_struct_type;
  auto* ptr_ty = llvm::PointerType::getUnqual(context);
  auto* i32_ty = llvm::Type::getInt32Ty(context);
  llvm_struct_type.string_type = llvm::StructType::create(context, {ptr_ty, i32_ty}, "StringStruct");
  llvm_struct_type.u8list_type = llvm::StructType::create(context, {ptr_ty, i32_ty}, "U8ListStruct");
  llvm_struct_type.u16list_type = llvm::StructType::create(context, {ptr_ty, i32_ty}, "U16ListStruct");
  llvm_struct_type.u32list_type = llvm::StructType::create(context, {ptr_ty, i32_ty}, "U32ListStruct");
  llvm_struct_type.u64list_type = llvm::StructType::create(context, {ptr_ty, i32_ty}, "U64ListStruct");
  llvm_struct_type.i8list_type = llvm::StructType::create(context, {ptr_ty, i32_ty}, "I8ListStruct");
  llvm_struct_type.i16list_type = llvm::StructType::create(context, {ptr_ty, i32_ty}, "I16ListStruct");
  llvm_struct_type.i32list_type = llvm::StructType::create(context, {ptr_ty, i32_ty}, "I32ListStruct");
  llvm_struct_type.i64list_type = llvm::StructType::create(context, {ptr_ty, i32_ty}, "I64ListStruct");
  llvm_struct_type.f32list_type = llvm::StructType::create(context, {ptr_ty, i32_ty}, "F32ListStruct");
  llvm_struct_type.f64list_type = llvm::StructType::create(context, {ptr_ty, i32_ty}, "F64ListStruct");
  llvm_struct_type.stringlist_type = llvm::StructType::create(context, {ptr_ty, i32_ty}, "StringListStruct");
  return llvm_struct_type;
}

Status GetEntryFunctionCallee(llvm::LLVMContext& context, std::unique_ptr<llvm::Module>& m,
                              const LLVMStructType& llvm_struct_type, const std::string& func_name, ValueType ret_type,
                              llvm::FunctionCallee* entry_func_callee) {
  auto* ptr_ty = llvm::PointerType::getUnqual(context);
  switch (ret_type) {
    case ValueType::kVoid: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm::Type::getVoidTy(context), ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kU8:
    case ValueType::kI8: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm::Type::getInt8Ty(context), ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kU16:
    case ValueType::kI16: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm::Type::getInt16Ty(context), ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kU32:
    case ValueType::kI32: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm::Type::getInt32Ty(context), ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kU64:
    case ValueType::kI64: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm::Type::getInt64Ty(context), ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kF32: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm::Type::getFloatTy(context), ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kF64: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm::Type::getDoubleTy(context), ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kString: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm_struct_type.string_type, ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kU8List: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm_struct_type.u8list_type, ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kU16List: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm_struct_type.u16list_type, ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kU32List: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm_struct_type.u32list_type, ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kU64List: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm_struct_type.u64list_type, ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kI8List: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm_struct_type.i8list_type, ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kI16List: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm_struct_type.i16list_type, ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kI32List: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm_struct_type.i32list_type, ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kI64List: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm_struct_type.i64list_type, ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kF32List: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm_struct_type.f32list_type, ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kF64List: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm_struct_type.f64list_type, ptr_ty, ptr_ty, ptr_ty);
    } break;
    case ValueType::kStringList: {
      *entry_func_callee = m->getOrInsertFunction(func_name, llvm_struct_type.stringlist_type, ptr_ty, ptr_ty, ptr_ty);
    } break;
    default:
      return Status::RuntimeError(
          "[internal] unknown return type for entry function: ", TypeHelper::TypeToString(ret_type), " (compiler bug)");
  }
  return Status::OK();
}

struct CommutativeCallCanonicalizerPass : public llvm::PassInfoMixin<CommutativeCallCanonicalizerPass> {
  CommutativeCallCanonicalizerPass() = default;

  static llvm::PreservedAnalyses run(llvm::Function& f, llvm::FunctionAnalysisManager& /*AM*/) {
    bool changed = false;
    for (auto& bb : f) {
      for (auto& i : bb) {
        if (auto* ci = llvm::dyn_cast<llvm::CallInst>(&i)) {
          llvm::Function* called_func = ci->getCalledFunction();
          if (called_func == nullptr) {
            continue;
          }
          if (!called_func->hasFnAttribute(kCommutative)) {
            continue;
          }
          if (ci->arg_size() < 2) {
            continue;
          }
          llvm::Value* arg0 = ci->getArgOperand(0);
          llvm::Value* arg1 = ci->getArgOperand(1);
          if (arg0->getType() != arg1->getType()) {
            continue;
          }
          if (ShouldSwapArguments(arg0, arg1)) {
            ci->setArgOperand(0, arg1);
            ci->setArgOperand(1, arg0);
            changed = true;
          }
        }
      }
    }
    return changed ? llvm::PreservedAnalyses::none() : llvm::PreservedAnalyses::all();
  }

 private:
  static bool ShouldSwapArguments(llvm::Value* arg0, llvm::Value* arg1) {
    return std::greater<llvm::Value*>()(arg0, arg1);
  }
};

Status BuildEntryFunction(llvm::LLVMContext& llvm_context, std::unique_ptr<llvm::Module>& owner,
                          const LLVMStructType& llvm_struct_type, const std::string& func_name, ValueType ret_type,
                          ExecNode* exec_node, const std::unique_ptr<FunctionRegistry>& func_registry) {
  llvm::FunctionCallee entry_func_callee;
  JF_RETURN_NOT_OK(
      GetEntryFunctionCallee(llvm_context, owner, llvm_struct_type, func_name, ret_type, &entry_func_callee));
  if (auto* entry_function = llvm::dyn_cast<llvm::Function>(entry_func_callee.getCallee())) {
    entry_function->addAttributeAtIndex(1, llvm::Attribute::get(llvm_context, llvm::Attribute::NoAlias));
    entry_function->addAttributeAtIndex(1, llvm::Attribute::get(llvm_context, llvm::Attribute::ReadOnly));
    entry_function->addAttributeAtIndex(2, llvm::Attribute::get(llvm_context, llvm::Attribute::NoAlias));
    entry_function->addAttributeAtIndex(3, llvm::Attribute::get(llvm_context, llvm::Attribute::NoAlias));
  }

  auto* entry_function = llvm::cast<llvm::Function>(entry_func_callee.getCallee());
  llvm::BasicBlock* entry_bb = llvm::BasicBlock::Create(llvm_context, "entryBB", entry_function);
  llvm::IRBuilder<> builder(entry_bb);
  std::unique_ptr<IRCodeGenContext> codegen_ctx = std::make_unique<IRCodeGenContext>(
      llvm_context, *owner, builder, entry_function, llvm_struct_type, func_registry);

  CodeGen codegen(*codegen_ctx);

  llvm::Value* ret_value;
  JF_RETURN_NOT_OK(codegen.GetValue(exec_node, &ret_value));
  if (ValueType::kVoid == ret_type) {
    builder.CreateRetVoid();
  } else {
    builder.CreateRet(ret_value);
  }
  return Status::OK();
}

}  // namespace

Status JitCore::CreateJit() {
  static auto machine = LLVMInit();
  const auto fp_mode = option_.fp_math_mode;
  auto jit_or_err =
      llvm::orc::LLJITBuilder()
          .setCompileFunctionCreator([fp_mode](llvm::orc::JITTargetMachineBuilder jtmb)
                                         -> llvm::Expected<std::unique_ptr<llvm::orc::IRCompileLayer::IRCompiler>> {
            jtmb.setCodeGenOptLevel(llvm::CodeGenOptLevel::Aggressive);
            if (fp_mode == FPMathMode::kFast) {
              jtmb.getOptions().AllowFPOpFusion = llvm::FPOpFusion::Fast;
              jtmb.getOptions().UnsafeFPMath = true;
            }
            return std::make_unique<llvm::orc::ConcurrentIRCompiler>(std::move(jtmb));
          })
          .create();
  if (!jit_or_err) {
    return Status::RuntimeError("Failed to create LLJIT: ", llvm::toString(jit_or_err.takeError()));
  }
  jit_ = std::move(*jit_or_err);
  auto dlsg = llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(jit_->getDataLayout().getGlobalPrefix());
  if (!dlsg) {
    return Status::RuntimeError("Failed to create DynamicLibrarySearchGenerator: ", llvm::toString(dlsg.takeError()));
  }
  jit_->getMainJITDylib().addGenerator(std::move(*dlsg));
  return Status::OK();
}

Status JitCore::BuildAndLink(const std::vector<ExecNode*>& nodes, const std::vector<std::string>& symbol_names,
                             const std::unique_ptr<FunctionRegistry>& func_registry, std::vector<char*>* out_func_ptrs,
                             std::vector<ValueType>* out_ret_types) {
  if (nodes.empty()) {
    return Status::ParseError("BuildAndLink: nodes is empty");
  }
  if (nodes.size() != symbol_names.size()) {
    return Status::RuntimeError("[internal] BuildAndLink: nodes.size() != symbol_names.size() (compiler bug)");
  }

  // Validation runs up front on caller-supplied nodes.
  Validator validator(func_registry);
  for (auto* node : nodes) {
    JF_RETURN_NOT_OK(validator.Validate(node));
  }

  Reset();

  auto llvm_context = std::make_unique<llvm::LLVMContext>();
  auto owner = std::make_unique<llvm::Module>("module", *llvm_context);
  LLVMStructType llvm_struct_type = CreateLLVMStructType(*llvm_context);

  out_ret_types->resize(nodes.size());
  for (size_t i = 0; i < nodes.size(); ++i) {
    ValueType node_ret_type = nodes[i]->GetReturnType();
    (*out_ret_types)[i] = node_ret_type;
    JF_RETURN_NOT_OK(BuildEntryFunction(*llvm_context, owner, llvm_struct_type, symbol_names[i], node_ret_type,
                                        nodes[i], func_registry));
  }
  JF_RETURN_NOT_OK(func_registry->SetCFuncAttr(owner.get()));

  // Verify the module before handing it to ORC. A verifier failure here
  // almost always indicates a codegen-side bug and should be surfaced
  // with context rather than as a generic ORC materialization error
  // later.
  std::string error_info;
  llvm::raw_string_ostream error_stream(error_info);
  if (llvm::verifyModule(*owner, &error_stream)) {
    return Status::RuntimeError("Module verification failed: ", error_info);
  }

  JF_RETURN_NOT_OK(CreateJit());

  if (option_.enable_save_compiled) {
    jit_->getObjTransformLayer().setTransform(
        [this](std::unique_ptr<llvm::MemoryBuffer> obj_buf) -> llvm::Expected<std::unique_ptr<llvm::MemoryBuffer>> {
          if (obj_buf) {
            llvm::StringRef data = obj_buf->getBuffer();
            compiled_object_bytes_.assign(data.data(), data.size());
          }
          return obj_buf;
        });
  }
  // optimize
  jit_->getIRTransformLayer().setTransform(
      [this](llvm::orc::ThreadSafeModule tsm, const llvm::orc::MaterializationResponsibility& /*r*/) {
        tsm.withModuleDo([this](llvm::Module& module) {
          llvm::PassBuilder pb;

          llvm::LoopAnalysisManager lam;
          llvm::FunctionAnalysisManager fam;
          llvm::CGSCCAnalysisManager cgam;
          llvm::ModuleAnalysisManager mam;

          pb.registerPeepholeEPCallback([&](llvm::FunctionPassManager& fpm, llvm::OptimizationLevel /*level*/) {
            fpm.addPass(CommutativeCallCanonicalizerPass());
          });

          // Register all the basic analyses with the managers.
          pb.registerModuleAnalyses(mam);
          pb.registerCGSCCAnalyses(cgam);
          pb.registerFunctionAnalyses(fam);
          pb.registerLoopAnalyses(lam);

          pb.crossRegisterProxies(lam, fam, cgam, mam);

          auto mpm = pb.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O3);

          mpm.run(module, mam);
          if (option_.dump_ir) {
            llvm::raw_string_ostream string_os(ir_code_);
            module.print(string_os, nullptr);
          }
        });
        return tsm;
      });
  if (auto err = jit_->addIRModule(llvm::orc::ThreadSafeModule(std::move(owner), std::move(llvm_context)))) {
    return Status::RuntimeError("Failed to add module to JIT: ", llvm::toString(std::move(err)));
  }
  JF_RETURN_NOT_OK(func_registry->MappingToJIT(jit_.get()));

  out_func_ptrs->resize(symbol_names.size());
  for (size_t i = 0; i < symbol_names.size(); ++i) {
    auto off = jit_->lookup(symbol_names[i]);
    if (!off) {
      return Status::RuntimeError("Failed to lookup function: ", symbol_names[i],
                                  ", error: ", llvm::toString(off.takeError()));
    }
    char* func_ptr = nullptr;
    func_ptr += off->getValue();
    (*out_func_ptrs)[i] = func_ptr;
  }
  return Status::OK();
}

Status JitCore::LoadObject(std::string_view object_bytes, const std::vector<std::string>& symbol_names,
                           const std::unique_ptr<FunctionRegistry>& func_registry, std::vector<char*>* out_func_ptrs) {
  Reset();
  JF_RETURN_NOT_OK(CreateJit());

  auto obj_buf = llvm::MemoryBuffer::getMemBufferCopy(llvm::StringRef(object_bytes.data(), object_bytes.size()),
                                                      "jitfusion.loaded.o");
  if (auto err = jit_->addObjectFile(std::move(obj_buf))) {
    return Status::RuntimeError("LoadCompiled: addObjectFile failed: ", llvm::toString(std::move(err)));
  }
  JF_RETURN_NOT_OK(func_registry->MappingToJIT(jit_.get()));

  out_func_ptrs->resize(symbol_names.size());
  for (size_t i = 0; i < symbol_names.size(); ++i) {
    auto off = jit_->lookup(symbol_names[i]);
    if (!off) {
      return Status::RuntimeError("LoadCompiled: entry symbol '", symbol_names[i],
                                  "' not found: ", llvm::toString(off.takeError()),
                                  ". The FunctionRegistry is probably missing a function the artifact depends on.");
    }
    char* fp = nullptr;
    fp += off->getValue();
    (*out_func_ptrs)[i] = fp;
  }
  return Status::OK();
}

void JitCore::Reset() {
  jit_.reset();
  ir_code_.clear();
  compiled_object_bytes_.clear();
}

}  // namespace jitfusion

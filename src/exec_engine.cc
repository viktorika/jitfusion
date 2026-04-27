/*
 * @Author: victorika
 * @Date: 2025-01-15 10:59:33
 * @Last Modified by: victorika
 * @Last Modified time: 2026-03-31 14:51:40
 */
#include "exec_engine.h"
#include <memory>
#include <string>
#include <vector>
#include "codegen/codegen.h"
#include "exec_node.h"
#include "function/function_init.h"
#include "function_registry.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
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
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/CallSiteSplitting.h"
#include "llvm/Transforms/Scalar/DCE.h"
#include "llvm/Transforms/Scalar/EarlyCSE.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/MergedLoadStoreMotion.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Vectorize/LoopVectorize.h"
#include "llvm/Transforms/Vectorize/SLPVectorizer.h"
#include "status.h"
#include "type.h"
#include "validator.h"

namespace jitfusion {

namespace {

const std::string kEntryFunctionName = "entry";

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
                              const LLVMStructType& llvm_struct_type, const std::string& func_name, ValueType ret_type,
                              llvm::FunctionCallee* entry_func_callee) {
  switch (ret_type) {
    case ValueType::kVoid: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm::Type::getVoidTy(context), llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU8:
    case ValueType::kI8: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm::Type::getInt8Ty(context), llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU16:
    case ValueType::kI16: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm::Type::getInt16Ty(context), llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU32:
    case ValueType::kI32: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm::Type::getInt32Ty(context), llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU64:
    case ValueType::kI64: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm::Type::getInt64Ty(context), llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kF32: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm::Type::getFloatTy(context), llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kF64: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm::Type::getDoubleTy(context), llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kString: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm_struct_type.string_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU8List: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm_struct_type.u8list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU16List: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm_struct_type.u16list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU32List: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm_struct_type.u32list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kU64List: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm_struct_type.u64list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kI8List: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm_struct_type.i8list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kI16List: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm_struct_type.i16list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kI32List: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm_struct_type.i32list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kI64List: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm_struct_type.i64list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kF32List: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm_struct_type.f32list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kF64List: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm_struct_type.f64list_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
    } break;
    case ValueType::kStringList: {
      *entry_func_callee = m->getOrInsertFunction(
          func_name, llvm_struct_type.stringlist_type, llvm::Type::getVoidTy(context)->getPointerTo(),
          llvm::Type::getVoidTy(context)->getPointerTo(), llvm::Type::getVoidTy(context)->getPointerTo());
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
                          const std::unique_ptr<ExecNode>& exec_node,
                          const std::unique_ptr<FunctionRegistry>& func_registry, Arena& const_value_arena_) {
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
      llvm_context, *owner, builder, entry_function, llvm_struct_type, func_registry, const_value_arena_);

  CodeGen codegen(*codegen_ctx);

  llvm::Value* ret_value;
  JF_RETURN_NOT_OK(codegen.GetValue(exec_node.get(), &ret_value));
  if (ValueType::kVoid == ret_type) {
    builder.CreateRetVoid();
  } else {
    builder.CreateRet(ret_value);
  }
  return Status::OK();
}

using return_void_function_type = void (*)(void*, void*, void*);
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
    : const_value_arena_(option.const_value_arena_alloc_min_chunk_size), jit_(nullptr), option_(option) {}

ExecEngine::~ExecEngine() {
  if (!jit_) {
    llvm::consumeError(jit_.takeError());
  }
}

Status ExecEngine::Compile(const std::unique_ptr<ExecNode>& exec_node,
                           const std::unique_ptr<FunctionRegistry>& func_registry) {
  // validator
  Validator validator(func_registry);
  JF_RETURN_NOT_OK(validator.Validate(exec_node.get()));
  ret_type_ = exec_node->GetReturnType();

  // codegen
  std::unique_ptr<llvm::LLVMContext> llvm_context = std::make_unique<llvm::LLVMContext>();
  std::unique_ptr<llvm::Module> owner = std::make_unique<llvm::Module>("module", *llvm_context);

  LLVMStructType llvm_struct_type = CreateLLVMStructType(*llvm_context);
  JF_RETURN_NOT_OK(BuildEntryFunction(*llvm_context, owner, llvm_struct_type, kEntryFunctionName, ret_type_, exec_node,
                                      func_registry, const_value_arena_));
  JF_RETURN_NOT_OK(func_registry->SetCFuncAttr(owner.get()));

  // verify
  std::string error_info;
  llvm::raw_string_ostream error_stream(error_info);
  if (llvm::verifyModule(*owner, &error_stream)) {
    return Status::RuntimeError("Module verification failed: ", error_info);
  }
  JF_RETURN_NOT_OK(CreateJitAndOptimize(func_registry, std::move(owner), std::move(llvm_context)));
  auto entry_func_offset = jit_->get()->lookup("entry");
  if (!entry_func_offset) {
    return Status::RuntimeError("Failed to find entry function");
  }
  entry_func_ptr_ = nullptr;
  entry_func_ptr_ += entry_func_offset->getValue();

  return Status::OK();
}

#define EXPAND_SWITCH_TYPE(func_ptr, ret_type)                                                              \
  switch (ret_type) {                                                                                       \
    case ValueType::kVoid: {                                                                                \
      reinterpret_cast<return_void_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);           \
    } break;                                                                                                \
    case ValueType::kU8: {                                                                                  \
      *result = reinterpret_cast<return_u8_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);   \
    } break;                                                                                                \
    case ValueType::kI8: {                                                                                  \
      *result = reinterpret_cast<return_i8_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);   \
    } break;                                                                                                \
    case ValueType::kU16: {                                                                                 \
      *result = reinterpret_cast<return_u16_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                \
    case ValueType::kI16: {                                                                                 \
      *result = reinterpret_cast<return_i16_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                \
    case ValueType::kU32: {                                                                                 \
      *result = reinterpret_cast<return_u32_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                \
    case ValueType::kI32: {                                                                                 \
      *result = reinterpret_cast<return_i32_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                \
    case ValueType::kU64: {                                                                                 \
      *result = reinterpret_cast<return_u64_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                \
    case ValueType::kI64: {                                                                                 \
      *result = reinterpret_cast<return_i64_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                \
    case ValueType::kF32: {                                                                                 \
      *result = reinterpret_cast<return_f32_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                \
    case ValueType::kF64: {                                                                                 \
      *result = reinterpret_cast<return_f64_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);  \
    } break;                                                                                                \
    case ValueType::kString: {                                                                              \
      StringStruct string =                                                                                 \
          reinterpret_cast<return_string_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);     \
      std::string res(string.data, string.len);                                                             \
      *result = std::move(res);                                                                             \
    } break;                                                                                                \
    case ValueType::kU8List: {                                                                              \
      U8ListStruct list =                                                                                   \
          reinterpret_cast<return_u8list_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);     \
      std::vector<uint8_t> res;                                                                             \
      res.resize(list.len);                                                                                 \
      memcpy(res.data(), list.data, list.len * sizeof(uint8_t));                                            \
      *result = std::move(res);                                                                             \
    } break;                                                                                                \
    case ValueType::kU16List: {                                                                             \
      U16ListStruct list =                                                                                  \
          reinterpret_cast<return_u16list_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<uint16_t> res;                                                                            \
      res.resize(list.len);                                                                                 \
      memcpy(res.data(), list.data, list.len * sizeof(uint16_t));                                           \
      *result = std::move(res);                                                                             \
    } break;                                                                                                \
    case ValueType::kU32List: {                                                                             \
      U32ListStruct list =                                                                                  \
          reinterpret_cast<return_u32list_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<uint32_t> res;                                                                            \
      res.resize(list.len);                                                                                 \
      memcpy(res.data(), list.data, list.len * sizeof(uint32_t));                                           \
      *result = std::move(res);                                                                             \
    } break;                                                                                                \
    case ValueType::kU64List: {                                                                             \
      U64ListStruct list =                                                                                  \
          reinterpret_cast<return_u64list_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<uint64_t> res;                                                                            \
      res.resize(list.len);                                                                                 \
      memcpy(res.data(), list.data, list.len * sizeof(uint64_t));                                           \
      *result = std::move(res);                                                                             \
    } break;                                                                                                \
    case ValueType::kI8List: {                                                                              \
      I8ListStruct list =                                                                                   \
          reinterpret_cast<return_i8list_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);     \
      std::vector<int8_t> res;                                                                              \
      res.resize(list.len);                                                                                 \
      memcpy(res.data(), list.data, list.len * sizeof(int8_t));                                             \
      *result = std::move(res);                                                                             \
    } break;                                                                                                \
    case ValueType::kI16List: {                                                                             \
      I16ListStruct list =                                                                                  \
          reinterpret_cast<return_i16list_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<int16_t> res;                                                                             \
      res.resize(list.len);                                                                                 \
      memcpy(res.data(), list.data, list.len * sizeof(int16_t));                                            \
      *result = std::move(res);                                                                             \
    } break;                                                                                                \
    case ValueType::kI32List: {                                                                             \
      I32ListStruct list =                                                                                  \
          reinterpret_cast<return_i32list_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<int32_t> res;                                                                             \
      res.resize(list.len);                                                                                 \
      memcpy(res.data(), list.data, list.len * sizeof(int32_t));                                            \
      *result = std::move(res);                                                                             \
    } break;                                                                                                \
    case ValueType::kI64List: {                                                                             \
      I64ListStruct list =                                                                                  \
          reinterpret_cast<return_i64list_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<int64_t> res;                                                                             \
      res.resize(list.len);                                                                                 \
      memcpy(res.data(), list.data, list.len * sizeof(int64_t));                                            \
      *result = std::move(res);                                                                             \
    } break;                                                                                                \
    case ValueType::kF32List: {                                                                             \
      F32ListStruct list =                                                                                  \
          reinterpret_cast<return_f32list_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<float> res;                                                                               \
      res.resize(list.len);                                                                                 \
      memcpy(res.data(), list.data, list.len * sizeof(float));                                              \
      *result = std::move(res);                                                                             \
    } break;                                                                                                \
    case ValueType::kF64List: {                                                                             \
      F64ListStruct list =                                                                                  \
          reinterpret_cast<return_f64list_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr);    \
      std::vector<double> res;                                                                              \
      res.resize(list.len);                                                                                 \
      memcpy(res.data(), list.data, list.len * sizeof(double));                                             \
      *result = std::move(res);                                                                             \
    } break;                                                                                                \
    case ValueType::kStringList: {                                                                          \
      StringListStruct list =                                                                               \
          reinterpret_cast<return_stringlist_function_type>(func_ptr)(entry_arguments, &exec_ctx, nullptr); \
      std::vector<std::string> res;                                                                         \
      res.resize(list.len);                                                                                 \
      for (uint32_t i = 0; i < list.len; ++i) {                                                             \
        res[i] = std::string(list.data[i].data, list.data[i].len);                                          \
      }                                                                                                     \
      *result = std::move(res);                                                                             \
    } break;                                                                                                \
    default:                                                                                                \
      return Status::RuntimeError("Unsupported return type");                                               \
  }

Status ExecEngine::Execute(void* entry_arguments, RetType* result) {
  ExecContext exec_ctx(option_.exec_ctx_arena_alloc_min_chunk_size);
  EXPAND_SWITCH_TYPE(entry_func_ptr_, ret_type_)
  if (exec_ctx.HasErrors()) {
    return Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  return Status::OK();
}

Status ExecEngine::Execute(ExecContext& exec_ctx, void* entry_arguments, RetType* result) {
  EXPAND_SWITCH_TYPE(entry_func_ptr_, ret_type_)
  Status ret = Status::OK();
  if (exec_ctx.HasErrors()) {
    ret = Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  exec_ctx.Clear();
  return ret;
}

Status ExecEngine::Execute(void* entry_arguments, void* result) {
  ExecContext exec_ctx(option_.exec_ctx_arena_alloc_min_chunk_size);
  reinterpret_cast<return_void_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, result);
  if (exec_ctx.HasErrors()) {
    return Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  return Status::OK();
}

Status ExecEngine::Execute(ExecContext& exec_ctx, void* entry_arguments, void* result) {
  reinterpret_cast<return_void_function_type>(entry_func_ptr_)(entry_arguments, &exec_ctx, result);
  Status ret = Status::OK();
  if (exec_ctx.HasErrors()) {
    ret = Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  exec_ctx.Clear();
  return ret;
}

Status ExecEngine::BatchCompile(const std::vector<std::unique_ptr<ExecNode>>& exec_nodes,
                                const std::unique_ptr<FunctionRegistry>& func_registry) {
  if (exec_nodes.empty()) {
    return Status::ParseError("BatchCompile: exec_nodes is empty");
  }

  Validator validator(func_registry);
  for (const auto& exec_node : exec_nodes) {
    JF_RETURN_NOT_OK(validator.Validate(exec_node.get()));
  }

  std::unique_ptr<llvm::LLVMContext> llvm_context = std::make_unique<llvm::LLVMContext>();
  std::unique_ptr<llvm::Module> owner = std::make_unique<llvm::Module>("module", *llvm_context);
  LLVMStructType llvm_struct_type = CreateLLVMStructType(*llvm_context);

  batch_ret_types_.resize(exec_nodes.size());
  std::vector<std::string> func_names;
  func_names.resize(exec_nodes.size());

  for (size_t i = 0; i < exec_nodes.size(); ++i) {
    std::string func_name = kEntryFunctionName + std::to_string(i);
    ValueType node_ret_type = exec_nodes[i]->GetReturnType();
    batch_ret_types_[i] = node_ret_type;

    JF_RETURN_NOT_OK(BuildEntryFunction(*llvm_context, owner, llvm_struct_type, func_name, node_ret_type, exec_nodes[i],
                                        func_registry, const_value_arena_));
    func_names[i] = std::move(func_name);
  }
  JF_RETURN_NOT_OK(func_registry->SetCFuncAttr(owner.get()));
  // verify
  std::string error_info;
  llvm::raw_string_ostream error_stream(error_info);
  if (llvm::verifyModule(*owner, &error_stream)) {
    return Status::RuntimeError("Module verification failed: ", error_info);
  }

  JF_RETURN_NOT_OK(CreateJitAndOptimize(func_registry, std::move(owner), std::move(llvm_context)));

  batch_entry_func_ptrs_.resize(func_names.size());
  for (size_t i = 0; i < func_names.size(); ++i) {
    auto func_offset = jit_->get()->lookup(func_names[i]);
    if (!func_offset) {
      return Status::RuntimeError("Failed to lookup function: ", func_names[i],
                                  ", error: ", llvm::toString(func_offset.takeError()));
    }
    char* func_ptr = nullptr;
    func_ptr += func_offset->getValue();
    batch_entry_func_ptrs_[i] = func_ptr;
  }

  return Status::OK();
}

Status ExecEngine::ExecuteAt(size_t index, void* entry_arguments, RetType* result) {
  if (index >= batch_entry_func_ptrs_.size()) {
    return Status::RuntimeError("ExecuteAt: index out of range");
  }
  ExecContext exec_ctx(option_.exec_ctx_arena_alloc_min_chunk_size);
  char* func_ptr = batch_entry_func_ptrs_[index];
  ValueType ret_type = batch_ret_types_[index];
  EXPAND_SWITCH_TYPE(func_ptr, ret_type)
  if (exec_ctx.HasErrors()) {
    return Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  return Status::OK();
}

Status ExecEngine::ExecuteAt(size_t index, ExecContext& exec_ctx, void* entry_arguments, RetType* result) {
  if (index >= batch_entry_func_ptrs_.size()) {
    return Status::RuntimeError("ExecuteAt: index out of range");
  }
  char* func_ptr = batch_entry_func_ptrs_[index];
  ValueType ret_type = batch_ret_types_[index];
  EXPAND_SWITCH_TYPE(func_ptr, ret_type)
  Status ret = Status::OK();
  if (exec_ctx.HasErrors()) {
    ret = Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  exec_ctx.Clear();
  return ret;
}

Status ExecEngine::ExecuteAt(size_t index, void* entry_arguments, void* result) {
  if (index >= batch_entry_func_ptrs_.size()) {
    return Status::RuntimeError("ExecuteAt: index out of range");
  }
  ExecContext exec_ctx(option_.exec_ctx_arena_alloc_min_chunk_size);
  reinterpret_cast<return_void_function_type>(batch_entry_func_ptrs_[index])(entry_arguments, &exec_ctx, result);
  if (exec_ctx.HasErrors()) {
    return Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  return Status::OK();
}

Status ExecEngine::ExecuteAt(size_t index, ExecContext& exec_ctx, void* entry_arguments, void* result) {
  if (index >= batch_entry_func_ptrs_.size()) {
    return Status::RuntimeError("ExecuteAt: index out of range");
  }
  reinterpret_cast<return_void_function_type>(batch_entry_func_ptrs_[index])(entry_arguments, &exec_ctx, result);
  Status ret = Status::OK();
  if (exec_ctx.HasErrors()) {
    ret = Status::RuntimeError(exec_ctx.GetErrorMessage());
  }
  exec_ctx.Clear();
  return ret;
}

Status ExecEngine::ExecuteAll(void* entry_arguments, std::vector<RetType>* results) {
  results->resize(batch_entry_func_ptrs_.size());
  std::string all_errors;
  for (size_t i = 0; i < batch_entry_func_ptrs_.size(); ++i) {
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
Status ExecEngine::ExecuteAll(ExecContext& exec_ctx, void* entry_arguments, std::vector<RetType>* results) {
  results->resize(batch_entry_func_ptrs_.size());
  std::string all_errors;
  for (size_t i = 0; i < batch_entry_func_ptrs_.size(); ++i) {
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
Status ExecEngine::ExecuteAll(void* entry_arguments, void* results) {
  std::string all_errors;
  for (size_t i = 0; i < batch_entry_func_ptrs_.size(); ++i) {
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
Status ExecEngine::ExecuteAll(ExecContext& exec_ctx, void* entry_arguments, void* results) {
  std::string all_errors;
  for (size_t i = 0; i < batch_entry_func_ptrs_.size(); ++i) {
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

Status ExecEngine::CreateJitAndOptimize(const std::unique_ptr<FunctionRegistry>& func_registry,
                                        std::unique_ptr<llvm::Module> owner,
                                        std::unique_ptr<llvm::LLVMContext> llvm_context) {
  static auto machine = LLVMInit();
  const auto fp_mode = option_.fp_math_mode;
  jit_ = llvm::orc::LLJITBuilder()
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
  if (!jit_) {
    return Status::RuntimeError("Failed to create LLJIT: ", llvm::toString(jit_.takeError()));
  }
  auto& jd = jit_->get()->getMainJITDylib();
  auto dlsg =
      llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(jit_->get()->getDataLayout().getGlobalPrefix());
  if (!dlsg) {
    return Status::RuntimeError("Failed to create DynamicLibrarySearchGenerator: ", llvm::toString(dlsg.takeError()));
  }
  jd.addGenerator(std::move(*dlsg));
  // optimize
  jit_->get()->getIRTransformLayer().setTransform(
      [&, this](llvm::orc::ThreadSafeModule tsm, const llvm::orc::MaterializationResponsibility& /*r*/) {
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

          // Create the optimization pipeline.
          auto mpm = pb.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O3);

          mpm.run(module, mam);
          if (option_.dump_ir) {
            ir_code_.clear();
            llvm::raw_string_ostream string_os(ir_code_);
            module.print(string_os, nullptr);
          }
        });
        return tsm;
      });
  if (auto err = jit_->get()->addIRModule(llvm::orc::ThreadSafeModule(std::move(owner), std::move(llvm_context)))) {
    return Status::RuntimeError("Failed to add module to JIT: ", llvm::toString(std::move(err)));
  }
  JF_RETURN_NOT_OK(func_registry->MappingToJIT(jit_->get()));
  return Status::OK();
}

}  // namespace jitfusion
/*
 * @Author: victorika
 * @Date: 2025-03-04 18:41:57
 * @Last Modified by: victorika
 * @Last Modified time: 2025-03-04 19:04:03
 */

#include <optional>
#include <vector>
#include "exec_engine.h"
#include "function_registry.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Type.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {

int64_t Alloc(int64_t exec_context, int64_t size) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  return reinterpret_cast<int64_t>(exec_ctx->arena.Allocate(size));
}

void AllocAttributeSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::inaccessibleMemOnly());
  f->addAttributeAtIndex(llvm::AttributeList::FunctionIndex,
                         llvm::Attribute::get(f->getContext(), "alloc-family", "jitfusion"));
  f->addFnAttr(llvm::Attribute::getWithAllocSizeArgs(f->getContext(), 1, std::nullopt));
  f->addFnAttr(llvm::Attribute::get(f->getContext(), llvm::Attribute::AllocKind,
                                    static_cast<uint64_t>(llvm::AllocFnKind::Alloc)));
}

}  // namespace

Status InitMemoryInternalFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("alloc", {ValueType::kI64, ValueType::kU32}, ValueType::kI64),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(Alloc), nullptr, AllocAttributeSetter}));
  return Status::OK();
}

llvm::Value *CallAllocFunc(IRCodeGenContext &ctx, llvm::Value *size) {
  FunctionSignature sign{"alloc", {ValueType::kI64, ValueType::kU32}, ValueType::kI64};
  llvm::FunctionType *func_type =
      llvm::FunctionType::get(llvm::Type::getInt64Ty(ctx.context),
                              {llvm::Type::getInt64Ty(ctx.context), llvm::Type::getInt32Ty(ctx.context)}, false);
  auto func_callee = ctx.module.getOrInsertFunction(sign.ToString(), func_type);
  return ctx.builder.CreateCall(func_callee, {ctx.entry_function->getArg(1), size}, "call_alloc");
}

}  // namespace jitfusion
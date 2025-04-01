/*
 * @Author: victorika
 * @Date: 2025-03-04 18:41:57
 * @Last Modified by: victorika
 * @Last Modified time: 2025-03-04 19:04:03
 */

#include <vector>
#include "exec_engine.h"
#include "function_registry.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Type.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {

uint8_t *Alloc(void *exec_context, int64_t size) {
  auto *exec_ctx = reinterpret_cast<ExecContext *>(exec_context);
  return exec_ctx->arena.Allocate(size);
}

void AllocAttributeSetter(llvm::ExecutionEngine * /*engine*/, llvm::Module * /*m*/, llvm::Function *f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
  f->addAttributeAtIndex(llvm::AttributeList::ReturnIndex,
                         llvm::Attribute::get(f->getContext(), llvm::Attribute::NoAlias));
}

}  // namespace

Status InitMemoryInternalFunc(FunctionRegistry *reg) {
  JF_RETURN_NOT_OK(
      reg->RegisterFunc(FunctionSignature("alloc", {ValueType::kPtr, ValueType::kU32}, ValueType::kPtr),
                        {FunctionType::kCFunc, reinterpret_cast<void *>(Alloc), nullptr, AllocAttributeSetter}));
  return Status::OK();
}

llvm::Value *CallAllocFunc(IRCodeGenContext &ctx, llvm::Value *size) {
  FunctionSignature sign{"alloc", {ValueType::kPtr, ValueType::kU32}, ValueType::kPtr};
  llvm::FunctionType *func_type = llvm::FunctionType::get(
      llvm::Type::getVoidTy(ctx.context)->getPointerTo(),
      {llvm::Type::getVoidTy(ctx.context)->getPointerTo(), llvm::Type::getInt32Ty(ctx.context)}, false);
  auto func_callee = ctx.module.getOrInsertFunction(sign.ToString(), func_type);
  return ctx.builder.CreateCall(func_callee, {ctx.entry_function->getArg(1), size}, "call_alloc");
}

}  // namespace jitfusion
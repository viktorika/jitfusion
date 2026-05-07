/*
 * @Author: victorika
 * @Date: 2025-01-21 10:38:03
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 14:23:50
 */
#include "codegen.h"
#include "codegen/const_global_emit.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Type.h"

namespace jitfusion {

struct MakeValueVisitor {
  explicit MakeValueVisitor(IRCodeGenContext &ctx) : ctx_(ctx) {}
  IRCodeGenContext &ctx_;
  llvm::Value *operator()(uint8_t v) const {
    return llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx_.context), v, false);
  }

  llvm::Value *operator()(uint16_t v) const {
    return llvm::ConstantInt::get(llvm::Type::getInt16Ty(ctx_.context), v, false);
  }

  llvm::Value *operator()(uint32_t v) const {
    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_.context), v, false);
  }

  llvm::Value *operator()(uint64_t v) const {
    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_.context), v, false);
  }

  llvm::Value *operator()(int8_t v) const {
    return llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx_.context), v, true);
  }

  llvm::Value *operator()(int16_t v) const {
    return llvm::ConstantInt::get(llvm::Type::getInt16Ty(ctx_.context), v, true);
  }

  llvm::Value *operator()(int32_t v) const {
    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_.context), v, true);
  }

  llvm::Value *operator()(int64_t v) const {
    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_.context), v, true);
  }

  llvm::Value *operator()(float v) const { return llvm::ConstantFP::get(llvm::Type::getFloatTy(ctx_.context), v); }

  llvm::Value *operator()(double v) const { return llvm::ConstantFP::get(llvm::Type::getDoubleTy(ctx_.context), v); }

  llvm::Value *operator()(const std::string &v) const {
    llvm::Constant *data_ptr = EmitStringPayloadGlobal(ctx_, v);
    return BuildPtrLenStructConstant(ctx_, ctx_.complex_type.string_type, data_ptr, static_cast<uint32_t>(v.size()));
  }
};

Status CodeGen::Visit(ConstantValueNode &const_node) {
  MakeValueVisitor visitor(ctx_);
  value_ = std::visit(visitor, const_node.GetVal());
  return Status::OK();
}

}  // namespace jitfusion
/*
 * @Author: victorika
 * @Date: 2025-01-21 10:38:03
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-21 16:37:45
 */
#include "codegen.h"

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
    char *str = reinterpret_cast<char *>(ctx_.const_value_arena.Allocate(static_cast<int>(v.size()) + 1));
    memcpy(str, v.data(), v.size());
    str[v.size()] = '\0';
    llvm::Constant *str_ptr =
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_.context), reinterpret_cast<int64_t>(str), false);
    llvm::Constant *str_len = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_.context), v.size(), false);
    std::vector<llvm::Constant *> struct_const_elements = {str_ptr, str_len};
    llvm::Value *ret = llvm::ConstantStruct::get(ctx_.complex_type, struct_const_elements);
    return ret;
  }
};

Status CodeGen::Visit(ConstantValueNode &const_node) {
  MakeValueVisitor visitor(ctx_);
  value_ = std::visit(visitor, const_node.GetVal());
  return Status::OK();
}

}  // namespace jitfusion
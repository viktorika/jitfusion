/*
 * @Author: victorika
 * @Date: 2025-01-21 16:16:20
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-21 17:00:59
 */
#include "codegen.h"

namespace jitfusion {

struct MakeListVisitor {
  explicit MakeListVisitor(IRCodeGenContext &ctx) : ctx_(ctx) {}

  IRCodeGenContext &ctx_;

  template <typename T>
  llvm::Value *operator()(const std::vector<T> &list) const {
    if constexpr (std::is_same_v<T, std::string>) {
      auto *data = reinterpret_cast<LLVMComplexStruct *>(
          ctx_.const_value_arena.Allocate(static_cast<int>(list.size()) * sizeof(LLVMComplexStruct)));

      for (size_t idx = 0; idx < list.size(); ++idx) {
        data[idx].data =
            reinterpret_cast<int64_t>(ctx_.const_value_arena.Allocate(static_cast<int>(list[idx].size()) + 1));
        memcpy(reinterpret_cast<char *>(data[idx].data), list[idx].data(), list[idx].size() * sizeof(char));
        reinterpret_cast<char *>(data[idx].data)[list[idx].size()] = '\0';
        data[idx].len = list[idx].size();
      }

      llvm::Constant *data_ptr =
          llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_.context), reinterpret_cast<int64_t>(data), false);
      llvm::Constant *data_len = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_.context), list.size(), false);

      std::vector<llvm::Constant *> struct_const_elements = {data_ptr, data_len};
      llvm::Value *ret = llvm::ConstantStruct::get(ctx_.complex_type, struct_const_elements);
      return ret;
    } else {
      void *data = ctx_.const_value_arena.Allocate(static_cast<int>(list.size()) * sizeof(T));
      memcpy(data, list.data(), list.size() * sizeof(T));

      llvm::Constant *data_ptr =
          llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_.context), reinterpret_cast<int64_t>(data), false);
      llvm::Constant *data_len = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_.context), list.size(), false);

      std::vector<llvm::Constant *> struct_const_elements = {data_ptr, data_len};
      llvm::Value *ret = llvm::ConstantStruct::get(ctx_.complex_type, struct_const_elements);
      return ret;
    }
  }
};

Status CodeGen::Visit(ConstantListValueNode &list_node) {
  MakeListVisitor visitor(ctx_);
  value_ = std::visit(visitor, list_node.GetValList());
  return Status::OK();
}

}  // namespace jitfusion
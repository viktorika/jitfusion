/*
 * @Author: victorika
 * @Date: 2025-01-21 16:16:20
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-21 17:00:59
 */
#include "codegen.h"
#include "llvm/IR/DerivedTypes.h"

namespace jitfusion {

struct MakeListVisitor {
  explicit MakeListVisitor(IRCodeGenContext &ctx) : ctx_(ctx) {}

  IRCodeGenContext &ctx_;

  llvm::Value *operator()(const std::vector<std::string> &list) const {
    auto *data = reinterpret_cast<StringStruct *>(
        ctx_.const_value_arena.Allocate(static_cast<int>(list.size()) * sizeof(StringStruct)));

    for (size_t idx = 0; idx < list.size(); ++idx) {
      data[idx].data =
          reinterpret_cast<char *>(ctx_.const_value_arena.Allocate(static_cast<int>(list[idx].size()) + 1));
      memcpy(data[idx].data, list[idx].data(), list[idx].size() * sizeof(char));
      data[idx].data[list[idx].size()] = '\0';
      data[idx].len = list[idx].size();
    }
    llvm::Constant *data_ptr =
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_.context), reinterpret_cast<int64_t>(data), false);
    data_ptr = llvm::ConstantExpr::getIntToPtr(data_ptr, ctx_.complex_type.string_type->getPointerTo());
    llvm::Constant *data_len = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_.context), list.size(), false);

    std::vector<llvm::Constant *> struct_const_elements = {data_ptr, data_len};
    llvm::Value *ret = llvm::ConstantStruct::get(ctx_.complex_type.stringlist_type, struct_const_elements);
    return ret;
  }

  template <typename ValueType>
  llvm::Value *CreateNumberList(const std::vector<ValueType> &list, llvm::PointerType *element_pointer_type,
                                llvm::StructType *struct_type) const {
    void *data = ctx_.const_value_arena.Allocate(static_cast<int>(list.size()) * sizeof(ValueType));
    memcpy(data, list.data(), list.size() * sizeof(ValueType));

    llvm::Constant *data_ptr =
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_.context), reinterpret_cast<int64_t>(data), false);
    data_ptr = llvm::ConstantExpr::getIntToPtr(data_ptr, element_pointer_type);
    llvm::Constant *data_len = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_.context), list.size(), false);

    std::vector<llvm::Constant *> struct_const_elements = {data_ptr, data_len};
    llvm::Value *ret = llvm::ConstantStruct::get(struct_type, struct_const_elements);
    return ret;
  }

  llvm::Value *operator()(const std::vector<uint8_t> &list) const {
    return CreateNumberList(list, llvm::Type::getInt8Ty(ctx_.context)->getPointerTo(), ctx_.complex_type.u8list_type);
  }
  llvm::Value *operator()(const std::vector<uint16_t> &list) const {
    return CreateNumberList(list, llvm::Type::getInt16Ty(ctx_.context)->getPointerTo(), ctx_.complex_type.u16list_type);
  }
  llvm::Value *operator()(const std::vector<uint32_t> &list) const {
    return CreateNumberList(list, llvm::Type::getInt32Ty(ctx_.context)->getPointerTo(), ctx_.complex_type.u32list_type);
  }
  llvm::Value *operator()(const std::vector<uint64_t> &list) const {
    return CreateNumberList(list, llvm::Type::getInt64Ty(ctx_.context)->getPointerTo(), ctx_.complex_type.u64list_type);
  }
  llvm::Value *operator()(const std::vector<int8_t> &list) const {
    return CreateNumberList(list, llvm::Type::getInt8Ty(ctx_.context)->getPointerTo(), ctx_.complex_type.i8list_type);
  }
  llvm::Value *operator()(const std::vector<int16_t> &list) const {
    return CreateNumberList(list, llvm::Type::getInt16Ty(ctx_.context)->getPointerTo(), ctx_.complex_type.i16list_type);
  }
  llvm::Value *operator()(const std::vector<int32_t> &list) const {
    return CreateNumberList(list, llvm::Type::getInt32Ty(ctx_.context)->getPointerTo(), ctx_.complex_type.i32list_type);
  }
  llvm::Value *operator()(const std::vector<int64_t> &list) const {
    return CreateNumberList(list, llvm::Type::getInt64Ty(ctx_.context)->getPointerTo(), ctx_.complex_type.i64list_type);
  }
  llvm::Value *operator()(const std::vector<float> &list) const {
    return CreateNumberList(list, llvm::Type::getFloatTy(ctx_.context)->getPointerTo(), ctx_.complex_type.f32list_type);
  }
  llvm::Value *operator()(const std::vector<double> &list) const {
    return CreateNumberList(list, llvm::Type::getDoubleTy(ctx_.context)->getPointerTo(),
                            ctx_.complex_type.f64list_type);
  }
};

Status CodeGen::Visit(ConstantListValueNode &list_node) {
  MakeListVisitor visitor(ctx_);
  value_ = std::visit(visitor, list_node.GetValList());
  return Status::OK();
}

}  // namespace jitfusion
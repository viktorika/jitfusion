/*
 * @Author: victorika
 * @Date: 2025-01-21 16:16:20
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 14:26:54
 */
#include "codegen.h"
#include "codegen/const_global_emit.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"

namespace jitfusion {

struct MakeListVisitor {
  explicit MakeListVisitor(IRCodeGenContext &ctx) : ctx_(ctx) {}

  IRCodeGenContext &ctx_;

  llvm::Value *operator()(const std::vector<std::string> &list) const {
    if (list.empty()) {
      auto *ptr_ty = llvm::PointerType::getUnqual(ctx_.context);
      return BuildPtrLenStructConstant(ctx_, ctx_.complex_type.stringlist_type, llvm::ConstantPointerNull::get(ptr_ty),
                                       0U);
    }

    auto *elem_struct_ty = ctx_.complex_type.string_type;
    std::vector<llvm::Constant *> elems;
    elems.reserve(list.size());
    for (const auto &s : list) {
      llvm::Constant *payload_ptr = EmitStringPayloadGlobal(ctx_, s);
      elems.emplace_back(BuildPtrLenStructConstant(ctx_, elem_struct_ty, payload_ptr, static_cast<uint32_t>(s.size())));
    }
    auto *arr_ty = llvm::ArrayType::get(elem_struct_ty, elems.size());
    auto *arr_init = llvm::ConstantArray::get(arr_ty, elems);
    auto *gv = new llvm::GlobalVariable(ctx_.module, arr_ty, /*isConstant=*/true, llvm::GlobalValue::PrivateLinkage,
                                        arr_init, "jf.strlist");
    gv->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
    gv->setAlignment(llvm::Align(alignof(void *)));
    auto *i32_ty = llvm::Type::getInt32Ty(ctx_.context);
    auto *gep = llvm::ConstantExpr::getInBoundsGetElementPtr(
        arr_ty, gv,
        llvm::ArrayRef<llvm::Constant *>{llvm::ConstantInt::get(i32_ty, 0), llvm::ConstantInt::get(i32_ty, 0)});
    return BuildPtrLenStructConstant(ctx_, ctx_.complex_type.stringlist_type, gep, static_cast<uint32_t>(list.size()));
  }

  template <typename ElemType>
  llvm::Value *CreateNumberList(const std::vector<ElemType> &list, llvm::StructType *struct_type) const {
    llvm::Constant *data_ptr =
        EmitTypedArrayGlobal(ctx_, llvm::ArrayRef<ElemType>(list.data(), list.size()), alignof(ElemType));
    return BuildPtrLenStructConstant(ctx_, struct_type, data_ptr, static_cast<uint32_t>(list.size()));
  }

  llvm::Value *operator()(const std::vector<uint8_t> &list) const {
    return CreateNumberList(list, ctx_.complex_type.u8list_type);
  }
  llvm::Value *operator()(const std::vector<uint16_t> &list) const {
    return CreateNumberList(list, ctx_.complex_type.u16list_type);
  }
  llvm::Value *operator()(const std::vector<uint32_t> &list) const {
    return CreateNumberList(list, ctx_.complex_type.u32list_type);
  }
  llvm::Value *operator()(const std::vector<uint64_t> &list) const {
    return CreateNumberList(list, ctx_.complex_type.u64list_type);
  }
  llvm::Value *operator()(const std::vector<int8_t> &list) const {
    return CreateNumberList(list, ctx_.complex_type.i8list_type);
  }
  llvm::Value *operator()(const std::vector<int16_t> &list) const {
    return CreateNumberList(list, ctx_.complex_type.i16list_type);
  }
  llvm::Value *operator()(const std::vector<int32_t> &list) const {
    return CreateNumberList(list, ctx_.complex_type.i32list_type);
  }
  llvm::Value *operator()(const std::vector<int64_t> &list) const {
    return CreateNumberList(list, ctx_.complex_type.i64list_type);
  }
  llvm::Value *operator()(const std::vector<float> &list) const {
    return CreateNumberList(list, ctx_.complex_type.f32list_type);
  }
  llvm::Value *operator()(const std::vector<double> &list) const {
    return CreateNumberList(list, ctx_.complex_type.f64list_type);
  }
};

Status CodeGen::Visit(ConstantListValueNode &list_node) {
  const auto &val_list = list_node.GetValList();
  auto it = ctx_.const_list_cache.find(val_list);
  if (it != ctx_.const_list_cache.end()) {
    value_ = it->second;
    return Status::OK();
  }

  MakeListVisitor visitor(ctx_);
  value_ = std::visit(visitor, val_list);
  ctx_.const_list_cache[val_list] = value_;
  return Status::OK();
}

}  // namespace jitfusion

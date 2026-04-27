/*
 * @Author: victorika
 * @Date: 2025-01-21 16:16:20
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-03 16:07:20
 */
#include "codegen.h"
#include "llvm/IR/DerivedTypes.h"

namespace jitfusion {

struct MakeListVisitor {
  explicit MakeListVisitor(IRCodeGenContext &ctx) : ctx_(ctx) {}

  IRCodeGenContext &ctx_;

  llvm::Value *operator()(const std::vector<std::string> &list) const {
    StringStruct *data = nullptr;
    if (!list.empty()) {
      data = reinterpret_cast<StringStruct *>(ctx_.const_value_arena.Allocate(list.size() * sizeof(StringStruct)));
      for (std::size_t idx = 0; idx < list.size(); ++idx) {
        if (list[idx].empty()) {
          data[idx].data = nullptr;
        } else {
          data[idx].data = reinterpret_cast<char *>(ctx_.const_value_arena.Allocate(list[idx].size()));
          memcpy(data[idx].data, list[idx].data(), list[idx].size() * sizeof(char));
        }
        data[idx].len = list[idx].size();
      }
    }
    return EmitPtrLenStruct(data, list.size(), ctx_.complex_type.stringlist_type);
  }

  template <typename ValueType>
  llvm::Value *CreateNumberList(const std::vector<ValueType> &list, llvm::StructType *struct_type) const {
    void *data = nullptr;
    if (!list.empty()) {
      data = ctx_.const_value_arena.Allocate(list.size() * sizeof(ValueType));
      memcpy(data, list.data(), list.size() * sizeof(ValueType));
    }
    return EmitPtrLenStruct(data, list.size(), struct_type);
  }

  llvm::Value *EmitPtrLenStruct(const void *data, std::size_t len, llvm::StructType *struct_type) const {
    auto &builder = ctx_.builder;
    llvm::Value *data_ptr = builder.CreateIntToPtr(builder.getInt64(reinterpret_cast<uintptr_t>(data)),
                                                   llvm::PointerType::getUnqual(ctx_.context));
    llvm::Value *ret = llvm::UndefValue::get(struct_type);
    ret = builder.CreateInsertValue(ret, data_ptr, 0);
    ret = builder.CreateInsertValue(ret, builder.getInt32(len), 1);
    return ret;
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
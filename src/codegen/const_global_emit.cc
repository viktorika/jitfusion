/*
 * @Author: victorika
 * @Date: 2026-05-07 14:19:55
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 14:25:09
 */
#include "codegen/const_global_emit.h"

#include <cstdint>
#include <type_traits>
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"

namespace jitfusion {

namespace {

llvm::GlobalVariable *CreatePrivateGlobal(llvm::Module &m, llvm::Constant *init, llvm::StringRef name_hint,
                                          unsigned alignment) {
  auto *gv = new llvm::GlobalVariable(m, init->getType(), /*isConstant=*/true, llvm::GlobalValue::PrivateLinkage, init,
                                      name_hint);
  gv->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
  if (alignment > 0) {
    gv->setAlignment(llvm::Align(alignment));
  }
  return gv;
}

llvm::Constant *AnchorArrayInGlobal(IRCodeGenContext &ctx, llvm::Constant *init, std::size_t alignment) {
  auto *gv = CreatePrivateGlobal(ctx.module, init, "jf.arr", static_cast<unsigned>(alignment == 0 ? 1 : alignment));
  auto *i32_ty = llvm::Type::getInt32Ty(ctx.context);
  return llvm::ConstantExpr::getInBoundsGetElementPtr(
      gv->getValueType(), gv,
      llvm::ArrayRef<llvm::Constant *>{llvm::ConstantInt::get(i32_ty, 0), llvm::ConstantInt::get(i32_ty, 0)});
}

llvm::Constant *NullIfEmpty(IRCodeGenContext &ctx, std::size_t n) {
  if (n == 0) {
    return llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(ctx.context));
  }
  return nullptr;
}

}  // namespace

llvm::Constant *EmitStringPayloadGlobal(IRCodeGenContext &ctx, const std::string &s) {
  auto *ptr_ty = llvm::PointerType::getUnqual(ctx.context);
  if (s.empty()) {
    return llvm::ConstantPointerNull::get(ptr_ty);
  }

  if (auto it = ctx.const_string_cache.find(s); it != ctx.const_string_cache.end()) {
    return it->second;
  }

  auto *init = llvm::ConstantDataArray::get(
      ctx.context, llvm::ArrayRef<uint8_t>(reinterpret_cast<const uint8_t *>(s.data()), s.size()));
  auto *gv = CreatePrivateGlobal(ctx.module, init, "jf.str", /*alignment=*/1);
  auto *gep = llvm::ConstantExpr::getInBoundsGetElementPtr(
      gv->getValueType(), gv,
      llvm::ArrayRef<llvm::Constant *>{llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx.context), 0),
                                       llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx.context), 0)});
  ctx.const_string_cache.emplace(s, gep);
  return gep;
}

template <typename ElemT>
llvm::Constant *EmitTypedArrayGlobal(IRCodeGenContext &ctx, llvm::ArrayRef<ElemT> data, std::size_t alignment) {
  static_assert(std::is_same_v<ElemT, std::uint8_t> || std::is_same_v<ElemT, std::uint16_t> ||
                    std::is_same_v<ElemT, std::uint32_t> || std::is_same_v<ElemT, std::uint64_t> ||
                    std::is_same_v<ElemT, std::int8_t> || std::is_same_v<ElemT, std::int16_t> ||
                    std::is_same_v<ElemT, std::int32_t> || std::is_same_v<ElemT, std::int64_t> ||
                    std::is_same_v<ElemT, float> || std::is_same_v<ElemT, double>,
                "EmitTypedArrayGlobal<ElemT>: ElemT must match one of the "
                "ConstantDataArray::get<T>() specializations (i8/i16/i32/i64 integer "
                "widths, signed or unsigned, plus float/double).");

  if (auto *null_c = NullIfEmpty(ctx, data.size())) {
    return null_c;
  }
  auto *init = llvm::ConstantDataArray::get(ctx.context, data);
  return AnchorArrayInGlobal(ctx, init, alignment);
}

template llvm::Constant *EmitTypedArrayGlobal<std::uint8_t>(IRCodeGenContext &, llvm::ArrayRef<std::uint8_t>,
                                                            std::size_t);
template llvm::Constant *EmitTypedArrayGlobal<std::uint16_t>(IRCodeGenContext &, llvm::ArrayRef<std::uint16_t>,
                                                             std::size_t);
template llvm::Constant *EmitTypedArrayGlobal<std::uint32_t>(IRCodeGenContext &, llvm::ArrayRef<std::uint32_t>,
                                                             std::size_t);
template llvm::Constant *EmitTypedArrayGlobal<std::uint64_t>(IRCodeGenContext &, llvm::ArrayRef<std::uint64_t>,
                                                             std::size_t);
template llvm::Constant *EmitTypedArrayGlobal<std::int8_t>(IRCodeGenContext &, llvm::ArrayRef<std::int8_t>,
                                                           std::size_t);
template llvm::Constant *EmitTypedArrayGlobal<std::int16_t>(IRCodeGenContext &, llvm::ArrayRef<std::int16_t>,
                                                            std::size_t);
template llvm::Constant *EmitTypedArrayGlobal<std::int32_t>(IRCodeGenContext &, llvm::ArrayRef<std::int32_t>,
                                                            std::size_t);
template llvm::Constant *EmitTypedArrayGlobal<std::int64_t>(IRCodeGenContext &, llvm::ArrayRef<std::int64_t>,
                                                            std::size_t);
template llvm::Constant *EmitTypedArrayGlobal<float>(IRCodeGenContext &, llvm::ArrayRef<float>, std::size_t);
template llvm::Constant *EmitTypedArrayGlobal<double>(IRCodeGenContext &, llvm::ArrayRef<double>, std::size_t);

llvm::Constant *BuildPtrLenStructConstant(IRCodeGenContext &ctx, llvm::StructType *struct_type,
                                          llvm::Constant *data_ptr, uint32_t len) {
  auto *i32_ty = llvm::Type::getInt32Ty(ctx.context);
  auto *len_c = llvm::ConstantInt::get(i32_ty, len, /*isSigned=*/false);
  assert(struct_type->getNumElements() == 2 && "PtrLen struct must have exactly 2 fields");
  return llvm::ConstantStruct::get(struct_type, {data_ptr, len_c});
}

}  // namespace jitfusion

/*
 * @Author: victorika
 * @Date: 2026-05-07 14:19:50
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 14:20:53
 */

#pragma once

#include <cstdint>
#include <string>
#include <type_traits>
#include "function_registry.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/GlobalVariable.h"

namespace jitfusion {

llvm::Constant *EmitStringPayloadGlobal(IRCodeGenContext &ctx, const std::string &s);

template <typename ElemT>
llvm::Constant *EmitTypedArrayGlobal(IRCodeGenContext &ctx, llvm::ArrayRef<ElemT> data, std::size_t alignment);

llvm::Constant *BuildPtrLenStructConstant(IRCodeGenContext &ctx, llvm::StructType *struct_type,
                                          llvm::Constant *data_ptr, uint32_t len);

}  // namespace jitfusion

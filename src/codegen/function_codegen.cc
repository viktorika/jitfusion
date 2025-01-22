/*
 * @Author: victorika
 * @Date: 2025-01-22 14:25:33
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-22 15:23:57
 */
#include <vector>
#include "codegen.h"
#include "function_registry.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

Status CodeGen::ValueTypeToLLVMType(ValueType value_type, llvm::Type** llvm_type) {
  static const std::unordered_map<ValueType, llvm::Type*> kValuetype2llvmtype{
      {ValueType::kU8, llvm::Type::getInt8Ty(ctx_.context)},
      {ValueType::kI8, llvm::Type::getInt8Ty(ctx_.context)},
      {ValueType::kU16, llvm::Type::getInt16Ty(ctx_.context)},
      {ValueType::kI16, llvm::Type::getInt16Ty(ctx_.context)},
      {ValueType::kU32, llvm::Type::getInt32Ty(ctx_.context)},
      {ValueType::kI32, llvm::Type::getInt32Ty(ctx_.context)},
      {ValueType::kU64, llvm::Type::getInt64Ty(ctx_.context)},
      {ValueType::kI64, llvm::Type::getInt64Ty(ctx_.context)},
      {ValueType::kF32, llvm::Type::getFloatTy(ctx_.context)},
      {ValueType::kF64, llvm::Type::getDoubleTy(ctx_.context)},
      {ValueType::kString, ctx_.complex_type},
      {ValueType::kU8List, ctx_.complex_type},
      {ValueType::kI8List, ctx_.complex_type},
      {ValueType::kU16List, ctx_.complex_type},
      {ValueType::kI16List, ctx_.complex_type},
      {ValueType::kU32List, ctx_.complex_type},
      {ValueType::kI32List, ctx_.complex_type},
      {ValueType::kU64List, ctx_.complex_type},
      {ValueType::kI64List, ctx_.complex_type},
      {ValueType::kF32List, ctx_.complex_type},
      {ValueType::kF64List, ctx_.complex_type},
      {ValueType::kStringList, ctx_.complex_type}};
  auto it = kValuetype2llvmtype.find(value_type);
  if (it == kValuetype2llvmtype.end()) {
    return Status::RuntimeError("ValueType ", TypeHelper::TypeToString(value_type), " ",
                                "can not convert to llvm type");
  }
  *llvm_type = it->second;
  return Status::OK();
}

Status CodeGen::Visit(FunctionNode& function_node) {
  std::vector<ValueType> args_type_list;
  std::vector<llvm::Type*> args_llvm_type_list;
  std::vector<llvm::Value*> args_llvm_value_list;
  const auto& args_list = function_node.GetArgs();
  args_type_list.reserve(args_list.size());
  args_llvm_type_list.reserve(args_list.size());
  args_llvm_value_list.reserve(args_list.size());
  for (const auto& args : args_list) {
    args_type_list.emplace_back(args->GetReturnType());
    llvm::Value* args_value;
    RETURN_NOT_OK(GetValue(args.get(), &args_value));
    args_llvm_type_list.emplace_back(args_value->getType());
    args_llvm_value_list.emplace_back(args_value);
  }

  FunctionSignature sign(function_node.GetFuncName(), args_type_list, ValueType::kUnknown);
  FunctionStructure func_struct;
  RETURN_NOT_OK(ctx_.function_registry->GetFuncBySign(sign, &func_struct));
  switch (func_struct.func_type) {
    case FunctionType::kLLVMIntrinicFunc: {
      value_ = func_struct.codegen_func(sign, args_llvm_type_list, args_llvm_value_list, ctx_);
    } break;
    case FunctionType::kCFunc: {
      llvm::Type* llvm_ret_type;
      RETURN_NOT_OK(ValueTypeToLLVMType(function_node.GetReturnType(), &llvm_ret_type));
      llvm::FunctionType* func_type =
          llvm::FunctionType::get(llvm_ret_type, llvm::ArrayRef<llvm::Type*>(args_llvm_type_list), false);

      llvm::FunctionCallee call_func_callee = ctx_.module.getOrInsertFunction(sign.ToString(), func_type);

      std::string error_info;
      llvm::raw_string_ostream error_stream(error_info);
      if (llvm::verifyFunction(llvm::cast<llvm::Function>(*call_func_callee.getCallee()), &error_stream)) {
        llvm::cast<llvm::Function>(*call_func_callee.getCallee()).print(error_stream);
        error_stream.flush();
        return Status::RuntimeError("Verify function failed in function_codegen in call_func_callee: " + error_info);
      }

      value_ = ctx_.builder.CreateCall(call_func_callee, args_llvm_value_list, "call_" + function_node.GetFuncName());
    } break;
    default:
      return Status::NotImplemented("Unsupport func type to codegen");
  }

  return Status::OK();
}

}  // namespace jitfusion
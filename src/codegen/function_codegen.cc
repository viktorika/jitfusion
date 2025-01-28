/*
 * @Author: victorika
 * @Date: 2025-01-22 14:25:33
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-22 16:50:36
 */
#include <vector>
#include "codegen.h"
#include "function_registry.h"
#include "llvm/IR/Type.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

Status CodeGen::ValueTypeToLLVMType(ValueType value_type, llvm::Type** llvm_type) {
  switch (value_type) {
    case ValueType::kU8:
    case ValueType::kI8: {
      *llvm_type = llvm::Type::getInt8Ty(ctx_.context);
    } break;
    case ValueType::kU16:
    case ValueType::kI16: {
      *llvm_type = llvm::Type::getInt16Ty(ctx_.context);
    } break;
    case ValueType::kU32:
    case ValueType::kI32: {
      *llvm_type = llvm::Type::getInt32Ty(ctx_.context);
    } break;
    case ValueType::kU64:
    case ValueType::kI64: {
      *llvm_type = llvm::Type::getInt64Ty(ctx_.context);
    } break;
    case ValueType::kF32: {
      *llvm_type = llvm::Type::getFloatTy(ctx_.context);
    } break;
    case ValueType::kF64: {
      *llvm_type = llvm::Type::getDoubleTy(ctx_.context);
    } break;
    case ValueType::kString:
    case ValueType::kU8List:
    case ValueType::kI8List:
    case ValueType::kU16List:
    case ValueType::kI16List:
    case ValueType::kU32List:
    case ValueType::kI32List:
    case ValueType::kU64List:
    case ValueType::kI64List:
    case ValueType::kF32List:
    case ValueType::kF64List:
    case ValueType::kStringList: {
      *llvm_type = ctx_.complex_type;
    } break;
    default:
      return Status::RuntimeError("ValueType ", TypeHelper::TypeToString(value_type), " ",
                                  "can not convert to llvm type");
  }
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
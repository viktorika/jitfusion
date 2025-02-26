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
    JF_RETURN_NOT_OK(GetValue(args.get(), &args_value));
    args_llvm_type_list.emplace_back(args_value->getType());
    args_llvm_value_list.emplace_back(args_value);
  }

  FunctionSignature sign(function_node.GetFuncName(), args_type_list, ValueType::kUnknown);
  FunctionStructure func_struct;
  JF_RETURN_NOT_OK(ctx_.function_registry->GetFuncBySign(sign, &func_struct));
  switch (func_struct.func_type) {
    case FunctionType::kLLVMIntrinicFunc: {
      value_ = func_struct.codegen_func(sign, args_llvm_type_list, args_llvm_value_list, ctx_);
    } break;
    case FunctionType::kCFunc: {
      llvm::Type* llvm_ret_type;
      JF_RETURN_NOT_OK(ValueTypeToLLVMType(ctx_, function_node.GetReturnType(), &llvm_ret_type));
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
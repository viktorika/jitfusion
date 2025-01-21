/*
 * @Author: victorika
 * @Date: 2025-01-20 14:16:13
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-21 19:05:52
 */
#pragma once

#include <type.h>
#include <memory>
#include "arena.h"
#include "exec_node.h"
#include "function_registry.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

namespace jitfusion {

struct IRCodeGenContext {
  IRCodeGenContext(llvm::LLVMContext &context, llvm::Module &module, llvm::IRBuilder<> &builder,
                   llvm::BasicBlock *entry_bb, llvm::Function *entry_function, llvm::StructType *complex_type,
                   const std::unique_ptr<FunctionRegistry> &function_registry, Arena &const_value_arena)
      : context(context),
        module(module),
        builder(builder),
        entry_bb(entry_bb),
        entry_function(entry_function),
        complex_type(complex_type),
        function_registry(function_registry),
        const_value_arena(const_value_arena) {}

  llvm::LLVMContext &context;
  llvm::Module &module;
  llvm::IRBuilder<> &builder;
  llvm::BasicBlock *entry_bb;
  llvm::Function *entry_function;
  llvm::StructType *complex_type;
  const std::unique_ptr<FunctionRegistry> &function_registry;
  Arena &const_value_arena;
};

class CodeGen : public Visitor {
  explicit CodeGen(IRCodeGenContext &ctx) : ctx_(ctx) {}

  Status GetValue(ExecNode *node, llvm::Value **ret_value);

  Status Visit(EntryArgumentNode &entry_argument_node) override;
  Status Visit(ExecContextNode &exec_context_node) override;
  Status Visit(ConstantValueNode &const_node) override;
  Status Visit(ConstantListValueNode &const_list_node) override;
  Status Visit(UnaryOPNode &unary_op_node) override;
  Status Visit(BinaryOPNode &binary_op_node) override;
  Status Visit(FunctionNode &function_node) override;

  Status NumericTypeConvert(ValueType from, ValueType to, llvm::Value **value);
  Status ToBoolean(ValueType from, llvm::Value **value);

 private:
  IRCodeGenContext &ctx_;
  llvm::Value *value_{nullptr};
};

}  // namespace jitfusion
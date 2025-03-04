/*
 * @Author: victorika
 * @Date: 2025-01-15 15:47:48
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-23 16:20:05
 */
#pragma once

#include <functional>
#include <unordered_map>
#include "arena.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "status.h"
#include "type.h"
#include <utility>
#include <vector>

namespace jitfusion {

class FunctionSignature;
class FunctionRegistry;
class FunctionRegistryFactory;

enum class FunctionType : uint8_t {
  kLLVMIntrinicFunc = 1,
  kCFunc = 2,
};

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

struct FunctionStructure {
  FunctionType func_type;
  void *c_func_ptr;
  std::function<llvm::Value *(const FunctionSignature &, const std::vector<llvm::Type *> &,
                              const std::vector<llvm::Value *> &, IRCodeGenContext &)>
      codegen_func;
  std::vector<std::pair<unsigned, llvm::Attribute>> attributes;
};

class FunctionSignature {
 public:
  FunctionSignature() = delete;
  FunctionSignature(std::string func_name, std::vector<ValueType> param_types, ValueType ret_type)
      : func_name_(std::move(func_name)), param_types_(std::move(param_types)), ret_type_(ret_type) {}

  bool operator==(const FunctionSignature &other) const;

  void SetRetType(ValueType ret_type);

  [[nodiscard]] size_t Hash() const;

  [[nodiscard]] ValueType GetRetType() const { return ret_type_; }

  [[nodiscard]] const std::string &GetName() const { return func_name_; }

  [[nodiscard]] const std::vector<ValueType> &GetparamTypes() const { return param_types_; }

  [[nodiscard]] std::string ToString() const;

 private:
  std::string func_name_;
  std::vector<ValueType> param_types_;
  ValueType ret_type_;
};

class FunctionRegistry {
 public:
  Status RegisterFunc(const FunctionSignature &func_sign, FunctionStructure func_struct);
  Status GetFuncBySign(FunctionSignature &func_sign, FunctionStructure *func_struct) const;
  Status MappingToLLVM(llvm::ExecutionEngine *engine, llvm::Module *m);

 private:
  FunctionRegistry() = default;
  struct KeyHash {
    std::size_t operator()(const FunctionSignature &k) const { return k.Hash(); }
  };

  struct KeyEquals {
    bool operator()(const FunctionSignature &s1, const FunctionSignature &s2) const { return s1 == s2; }
  };
  std::unordered_map<std::string, std::vector<std::tuple<FunctionSignature, FunctionStructure>>> name2funclist_;
  std::unordered_map<const FunctionSignature, FunctionStructure, KeyHash, KeyEquals> signature2funcstruct_;

  friend FunctionRegistryFactory;

  Status Init();
};

class FunctionRegistryFactory {
 public:
  static Status CreateFunctionRegistry(std::unique_ptr<FunctionRegistry> *func_registry) {
    *func_registry = std::unique_ptr<FunctionRegistry>(new FunctionRegistry());
    JF_RETURN_NOT_OK((*func_registry)->Init());
    return Status::OK();
  }
};

}  // namespace jitfusion
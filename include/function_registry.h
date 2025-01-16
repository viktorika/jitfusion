/*
 * @Author: victorika
 * @Date: 2025-01-15 15:47:48
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-16 16:50:15
 */
#pragma once

#include <functional>
#include "status.h"
#include "type.h"

namespace jitfusion {

class FunctionSignature;
class FunctionRegistry;
class FunctionRegistryFactory;

enum class FunctionType : uint8_t {
  kLLVMIntrinicFunc = 1,
  kCFunc = 2,
};

struct FunctionStructure {
  FunctionType func_type;
  void* c_func_ptr;
  std::function<void()> codegen_func;  // TODO(victorika): 待确认需要哪些参数
};

class FunctionSignature {
 public:
  FunctionSignature() = delete;
  FunctionSignature(std::string func_name, std::vector<ValueType> param_types, ValueType ret_type)
      : func_name_(std::move(func_name)), param_types_(std::move(param_types)), ret_type_(ret_type) {}

  bool operator==(const FunctionSignature& other) const;

  void SetRetType(ValueType ret_type);

  [[nodiscard]] size_t Hash() const;

  [[nodiscard]] ValueType GetRetType() const { return ret_type_; }

  [[nodiscard]] const std::string& GetName() const { return func_name_; }

  [[nodiscard]] const std::vector<ValueType>& GetparamTypes() const { return param_types_; }

  [[nodiscard]] std::string ToString() const;

 private:
  std::string func_name_;
  std::vector<ValueType> param_types_;
  ValueType ret_type_;
};

class FunctionRegistry {
 public:
  Status RegisterFunc(const FunctionSignature& func_sign, FunctionStructure func_struct);
  Status GetFuncBySign(FunctionSignature& func_sign, FunctionStructure* func_struct) const;

 private:
  FunctionRegistry() = default;
  struct KeyHash {
    std::size_t operator()(const FunctionSignature& k) const { return k.Hash(); }
  };

  struct KeyEquals {
    bool operator()(const FunctionSignature& s1, const FunctionSignature& s2) const { return s1 == s2; }
  };
  std::unordered_map<std::string, std::vector<std::tuple<FunctionSignature, FunctionStructure>>> name2funclist_;
  std::unordered_map<const FunctionSignature, FunctionStructure, KeyHash, KeyEquals> signature2funcstruct_;

  friend FunctionRegistryFactory;

  Status Init();
};

class FunctionRegistryFactory {
 public:
  static Status CreateFunctionRegistry(std::unique_ptr<FunctionRegistry>* func_registry) {
    *func_registry = std::unique_ptr<FunctionRegistry>(new FunctionRegistry());
    RETURN_NOT_OK((*func_registry)->Init());
    return Status::OK();
  }
};

}  // namespace jitfusion
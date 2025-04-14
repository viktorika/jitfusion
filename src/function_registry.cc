/*
 * @Author: victorika
 * @Date: 2025-01-15 14:26:36
 * @Last Modified by: victorika
 * @Last Modified time: 2025-03-04 19:03:28
 */
#include "function_registry.h"
#include <type_traits>
#include "function/function_init.h"
#include "status.h"
#include "type.h"

namespace jitfusion {

namespace {
#define HASH_ROTL32(x, r) (((x) << (r)) | ((x) >> (32 - (r))))

template <typename SizeT>
inline void hash_combine_impl(SizeT& seed, SizeT value) {
  if constexpr (std::is_same_v<SizeT, uint32_t>) {
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    value *= c1;
    value = HASH_ROTL32(value, 15);
    value *= c2;

    seed ^= value;
    seed = HASH_ROTL32(seed, 13);
    seed = seed * 5 + 0xe6546b64;
  } else {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
}

template <class T>
inline void hash_combine(std::size_t& seed, T const& v) {
  std::hash<T> hasher;
  return hash_combine_impl(seed, hasher(v));
}
}  // namespace

bool FunctionSignature::operator==(const FunctionSignature& other) const {
  if (param_types_.size() != other.param_types_.size() || func_name_ != other.func_name_) {
    return false;
  }
  for (size_t idx = 0; idx < param_types_.size(); idx++) {
    if (param_types_[idx] != other.param_types_[idx]) {
      return false;
    }
  }
  return true;
}

std::size_t FunctionSignature::Hash() const {
  static const size_t kSeedValue = 17;
  size_t result = kSeedValue;
  hash_combine(result, func_name_);
  for (const auto& param_type : param_types_) {
    hash_combine(result, static_cast<size_t>(param_type));
  }
  return result;
}

void FunctionSignature::SetRetType(ValueType ret_type) { ret_type_ = ret_type; }

std::string FunctionSignature::ToString() const {
  std::stringstream s;
  s << func_name_ << "(";
  for (size_t i = 0; i < param_types_.size(); i++) {
    if (i > 0) {
      s << ", ";
    }
    s << TypeHelper::TypeToString(param_types_[i]);
  }
  s << ")";
  return s.str();
}

Status FunctionRegistry::RegisterFunc(const FunctionSignature& func_sign, FunctionStructure func_struct) {
  if (FunctionType::kLLVMIntrinicFunc == func_struct.func_type && !func_struct.codegen_func) {
    return Status::InvalidArgument("llvm instrinic function must supply codegen function");
  }
  if (FunctionType::kCFunc == func_struct.func_type && nullptr == func_struct.c_func_ptr) {
    return Status::InvalidArgument("c function must supply the c function address");
  }
  name2funclist_[func_sign.GetName()].emplace_back(func_sign, func_struct);
  signature2funcstruct_[func_sign] = func_struct;
  return Status::OK();
}

Status FunctionRegistry::GetFuncBySign(FunctionSignature& func_sign, FunctionStructure* func_struct) const {
  auto iter = name2funclist_.find(func_sign.GetName());
  if (iter == name2funclist_.end()) {
    return Status::RuntimeError("function ", func_sign.ToString(), " not found");
  }
  for (const auto& [sign, fs] : iter->second) {
    if (func_sign == sign) {
      func_sign.SetRetType(sign.GetRetType());
      *func_struct = fs;
      return Status::OK();
    }
  }
  return Status::RuntimeError("function ", func_sign.ToString(), " not found");
}

Status FunctionRegistry::MappingToLLVM(llvm::ExecutionEngine* engine, llvm::Module* m) {
  if (engine == nullptr) {
    return Status::RuntimeError("engine pointer is nullptr");
  }
  for (const auto& [sign, fc] : signature2funcstruct_) {
    if (FunctionType::kLLVMIntrinicFunc == fc.func_type) {
      continue;
    }
    auto* func = m->getFunction(sign.ToString());
    if (func == nullptr) {
      continue;
    }
    if (fc.func_attr_setter) {
      fc.func_attr_setter(engine, m, func);
    }
    engine->addGlobalMapping(func, fc.c_func_ptr);
    // I don't know why I cant use sign string to add globalmapping
    // engine->addGlobalMapping(sign.ToString(), reinterpret_cast<uint64_t>(fc.c_func_ptr));
  }
  return Status::OK();
}

Status FunctionRegistry::Init() {
  JF_RETURN_NOT_OK(InitMathInternalFunc(this));
  JF_RETURN_NOT_OK(InitStringInternalFunc(this));
  JF_RETURN_NOT_OK(InitListInternalFunc(this));
  return Status::OK();
}

}  // namespace jitfusion

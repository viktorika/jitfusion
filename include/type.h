/*
 * @Author: victorika
 * @Date: 2025-01-14 16:21:53
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-14 16:22:46
 */
#pragma once

#include <cstdint>
#include <variant>
#include <vector>

namespace jitfusion {

enum class ExecNodeType : uint8_t {
  kConstValueNode = 1,
  kConstListValueNode = 2,
  kUnaryOPType = 3,
  kBinaryOPNode = 4,
  kFunctionNode = 5,
};

enum class BinaryOPType : uint8_t {
  kAdd = 1,
  kSub = 2,
  kMul = 3,
  kDiv = 4,
  kMod = 5,

  kLarge = 10,
  kLargeEqual = 11,
  kEqual = 12,
  kLess = 13,
  kLessEqual = 14,
  kNotEqual = 15,

  kAnd = 20,
  kOr = 21,

  kBitwiseAnd = 30,
  kBitwiseOr = 31,
  kBitwiseXor = 32,
  kBitwiseShiftLeft = 33,
  kBitwiseShiftRight = 34,
};

enum class UnaryOPType : uint8_t {
  kPlus = 1,
  kMinus = 2,

  kNot = 22,

  kBitwiseNot = 32,
};

enum class ValueType : uint8_t {
  kUnknown,
  kU8,
  kU16,
  kU32,
  kU64,
  kI8,
  kI16,
  kI32,
  kI64,
  kF32,
  kF64,
  kString,
  kU8List,
  kU16List,
  kU32List,
  kU64List,
  kI8List,
  kI16List,
  kI32List,
  kI64List,
  kF32List,
  kF64List,
  kStringList,
};

using ConstantValueType =
    std::variant<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, float, double, std::string>;

using ConstantListValueType =
    std::variant<std::vector<int8_t>, std::vector<int16_t>, std::vector<int32_t>, std::vector<int64_t>,
                 std::vector<uint8_t>, std::vector<uint16_t>, std::vector<uint32_t>, std::vector<uint64_t>,
                 std::vector<float>, std::vector<double>, std::vector<std::string>>;

inline std::string UnaryOPTypeToString(UnaryOPType type) {
  switch (type) {
    case UnaryOPType::kPlus:
      return "+";
    case UnaryOPType::kMinus:
      return "-";
    case UnaryOPType::kNot:
      return "!";
    case UnaryOPType::kBitwiseNot:
      return "~";
  }
  return "unknown";
}

inline std::string BinaryOPTypeToString(BinaryOPType type) {
  switch (type) {
    case BinaryOPType::kAdd:
      return "+";
    case BinaryOPType::kSub:
      return "-";
    case BinaryOPType::kMul:
      return "*";
    case BinaryOPType::kDiv:
      return "/";
    case BinaryOPType::kMod:
      return "%";
    case BinaryOPType::kLarge:
      return ">";
    case BinaryOPType::kLargeEqual:
      return ">=";
    case BinaryOPType::kEqual:
      return "==";
    case BinaryOPType::kLess:
      return "<";
    case BinaryOPType::kLessEqual:
      return "<=";
    case BinaryOPType::kNotEqual:
      return "!=";
    case BinaryOPType::kAnd:
      return "&&";
    case BinaryOPType::kOr:
      return "||";
    case BinaryOPType::kBitwiseAnd:
      return "&";
    case BinaryOPType::kBitwiseOr:
      return "|";
    case BinaryOPType::kBitwiseXor:
      return "^";
    case BinaryOPType::kBitwiseShiftLeft:
      return "<<";
    case BinaryOPType::kBitwiseShiftRight:
      return ">>";
  }
  return "unknown";
}


}  // namespace jitfusion
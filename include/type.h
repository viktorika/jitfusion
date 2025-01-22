/*
 * @Author: victorika
 * @Date: 2025-01-14 16:21:53
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-22 15:42:23
 */
#pragma once

#include <cstdint>
#include <variant>
#include <vector>

namespace jitfusion {

enum class ExecNodeType : uint8_t {
  kEntryArgumentNode = 1,
  kExecContextNode = 2,
  kConstValueNode = 3,
  kConstListValueNode = 4,
  kUnaryOPType = 5,
  kBinaryOPNode = 6,
  kFunctionNode = 7,
  kNoOPNode = 8,
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

struct LLVMComplexStruct {
  int64_t data;
  uint32_t len;
};

class TypeHelper {
 public:
  static std::string UnaryOPTypeToString(UnaryOPType type);
  static std::string BinaryOPTypeToString(BinaryOPType type);
  static std::string TypeToString(ValueType type);

  static bool IsNumberType(ValueType type);
  static bool IsIntegerType(ValueType type);
  static bool IsSignedType(ValueType type);
  static bool IsFloatType(ValueType type);
  static bool IsComplexType(ValueType type);

  static bool IsRelationalBinaryOPType(BinaryOPType type);
  static bool IsLogicalBinaryOPType(BinaryOPType type);

  static ValueType GetPromotedType(ValueType lhs, ValueType rhs);
  static int GetBitWidthFromType(ValueType type);
};

}  // namespace jitfusion
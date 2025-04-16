/*
 * @Author: victorika
 * @Date: 2025-01-14 16:21:53
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-24 11:01:49
 */
#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace jitfusion {

enum class ExecNodeType : uint8_t {
  kEntryArgumentNode = 1,
  kExecContextNode = 2,
  kOutputNode = 3,
  kConstValueNode = 4,
  kConstListValueNode = 5,
  kUnaryOPNode = 6,
  kBinaryOPNode = 7,
  kFunctionNode = 8,
  kNoOPNode = 9,
  kIfNode = 10,
  kSwitchNode = 11,
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
  kPtr,
};

using ConstantValueType =
    std::variant<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, float, double, std::string>;

using ConstantListValueType =
    std::variant<std::vector<int8_t>, std::vector<int16_t>, std::vector<int32_t>, std::vector<int64_t>,
                 std::vector<uint8_t>, std::vector<uint16_t>, std::vector<uint32_t>, std::vector<uint64_t>,
                 std::vector<float>, std::vector<double>, std::vector<std::string>>;

struct StringStruct {
  using CElementType = char;
  static constexpr ValueType kElementType = ValueType::kI8;
  static constexpr uint32_t kElementSize = sizeof(char);
  char* data;
  uint32_t len;
};

struct U8ListStruct {
  using CElementType = uint8_t;
  static constexpr ValueType kElementType = ValueType::kU8;
  static constexpr uint32_t kElementSize = sizeof(uint8_t);
  uint8_t* data;
  uint32_t len;
};

struct U16ListStruct {
  using CElementType = uint16_t;
  static constexpr ValueType kElementType = ValueType::kU16;
  static constexpr uint32_t kElementSize = sizeof(uint16_t);
  uint16_t* data;
  uint32_t len;
};

struct U32ListStruct {
  using CElementType = uint32_t;
  static constexpr ValueType kElementType = ValueType::kU32;
  static constexpr uint32_t kElementSize = sizeof(uint32_t);
  uint32_t* data;
  uint32_t len;
};

struct U64ListStruct {
  using CElementType = uint64_t;
  static constexpr ValueType kElementType = ValueType::kU64;
  static constexpr uint32_t kElementSize = sizeof(uint64_t);
  uint64_t* data;
  uint32_t len;
};

struct I8ListStruct {
  using CElementType = int8_t;
  static constexpr ValueType kElementType = ValueType::kI8;
  static constexpr uint32_t kElementSize = sizeof(int8_t);
  int8_t* data;
  uint32_t len;
};

struct I16ListStruct {
  using CElementType = int16_t;
  static constexpr ValueType kElementType = ValueType::kI16;
  static constexpr uint32_t kElementSize = sizeof(int16_t);
  int16_t* data;
  uint32_t len;
};

struct I32ListStruct {
  using CElementType = int32_t;
  static constexpr ValueType kElementType = ValueType::kI32;
  static constexpr uint32_t kElementSize = sizeof(int32_t);
  int32_t* data;
  uint32_t len;
};

struct I64ListStruct {
  using CElementType = int64_t;
  static constexpr ValueType kElementType = ValueType::kI64;
  static constexpr uint32_t kElementSize = sizeof(int64_t);
  int64_t* data;
  uint32_t len;
};

struct F32ListStruct {
  using CElementType = float;
  static constexpr ValueType kElementType = ValueType::kF32;
  static constexpr uint32_t kElementSize = sizeof(float);
  float* data;
  uint32_t len;
};

struct F64ListStruct {
  using CElementType = double;
  static constexpr ValueType kElementType = ValueType::kF64;
  static constexpr uint32_t kElementSize = sizeof(double);
  double* data;
  uint32_t len;
};

struct StringListStruct {
  using CElementType = StringStruct;
  static constexpr ValueType kElementType = ValueType::kStringList;
  static constexpr uint32_t kElementSize = sizeof(StringStruct);
  StringStruct* data;
  uint32_t len;
};

using RetType = std::variant<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double,
                             std::vector<uint8_t>, std::vector<uint16_t>, std::vector<uint32_t>, std::vector<uint64_t>,
                             std::vector<int8_t>, std::vector<int16_t>, std::vector<int32_t>, std::vector<int64_t>,
                             std::vector<float>, std::vector<double>, std::vector<std::string>, std::string>;

class TypeHelper {
 public:
  static std::string UnaryOPTypeToString(UnaryOPType type);
  static std::string BinaryOPTypeToString(BinaryOPType type);
  static std::string TypeToString(ValueType type);

  static bool IsNumericType(ValueType type);
  static bool IsIntegerType(ValueType type);
  static bool IsSignedType(ValueType type);
  static bool IsFloatType(ValueType type);
  static bool IsComplexType(ValueType type);

  static bool IsRelationalBinaryOPType(BinaryOPType type);
  static bool IsLogicalBinaryOPType(BinaryOPType type);

  static ValueType GetPromotedType(ValueType lhs, ValueType rhs);
  static int GetBitWidthFromType(ValueType type);

  static uint32_t GetElementSize(ValueType type);
  static ValueType GetElementType(ValueType type);
};

}  // namespace jitfusion
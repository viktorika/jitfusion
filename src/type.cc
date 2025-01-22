/*
 * @Author: victorika
 * @Date: 2025-01-16 16:59:03
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-22 10:32:31
 */
#pragma once

#include <type.h>
#include <unordered_map>

namespace jitfusion {

std::string TypeHelper::UnaryOPTypeToString(UnaryOPType type) {
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

std::string TypeHelper::BinaryOPTypeToString(BinaryOPType type) {
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

std::string TypeHelper::TypeToString(ValueType type) {
  switch (type) {
    case ValueType::kUnknown:
      return "unknown";
    case ValueType::kU8:
      return "u8";
    case ValueType::kU16:
      return "u16";
    case ValueType::kU32:
      return "u32";
    case ValueType::kU64:
      return "u64";
    case ValueType::kI8:
      return "i8";
    case ValueType::kI16:
      return "i16";
    case ValueType::kI32:
      return "i32";
    case ValueType::kI64:
      return "i64";
    case ValueType::kF32:
      return "f32";
    case ValueType::kF64:
      return "f64";
    case ValueType::kString:
      return "string";
    case ValueType::kU8List:
      return "u8list";
    case ValueType::kU16List:
      return "u16list";
    case ValueType::kU32List:
      return "u32list";
    case ValueType::kU64List:
      return "u64list";
    case ValueType::kI8List:
      return "i8list";
    case ValueType::kI16List:
      return "i16list";
    case ValueType::kI32List:
      return "i32list";
    case ValueType::kI64List:
      return "i64list";
    case ValueType::kF32List:
      return "f32list";
    case ValueType::kF64List:
      return "f64list";
    case ValueType::kStringList:
      return "stringlist";
  }
  return "unknown";
}

bool TypeHelper::IsNumberType(ValueType type) {
  switch (type) {
    case ValueType::kF32:
    case ValueType::kF64:
    case ValueType::kI8:
    case ValueType::kI16:
    case ValueType::kI32:
    case ValueType::kI64:
    case ValueType::kU8:
    case ValueType::kU16:
    case ValueType::kU32:
    case ValueType::kU64:
      return true;
    default:
      return false;
  }
  return false;
}

bool TypeHelper::IsIntegerType(ValueType type) {
  switch (type) {
    case ValueType::kI8:
    case ValueType::kI16:
    case ValueType::kI32:
    case ValueType::kI64:
    case ValueType::kU8:
    case ValueType::kU16:
    case ValueType::kU32:
    case ValueType::kU64:
      return true;
    default:
      return false;
  }
  return false;
}

ValueType TypeHelper::GetPromotedType(ValueType lhs, ValueType rhs) {
  static const std::unordered_map<ValueType, int32_t> kWeight = {
      {ValueType::kUnknown, -1}, {ValueType::kI8, 0},       {ValueType::kU8, 1},         {ValueType::kI16, 2},
      {ValueType::kU16, 3},      {ValueType::kI32, 4},      {ValueType::kU32, 5},        {ValueType::kI64, 6},
      {ValueType::kU64, 7},      {ValueType::kF32, 8},      {ValueType::kF64, 9},        {ValueType::kString, 10},
      {ValueType::kI8List, 11},  {ValueType::kU8List, 12},  {ValueType::kI16List, 13},   {ValueType::kU16List, 14},
      {ValueType::kI32List, 15}, {ValueType::kU32List, 16}, {ValueType::kI64List, 17},   {ValueType::kU64List, 18},
      {ValueType::kF32List, 19}, {ValueType::kF64List, 20}, {ValueType::kStringList, 21}};
  auto left_weight = kWeight.at(lhs);
  auto right_weight = kWeight.at(rhs);
  return left_weight > right_weight ? lhs : rhs;
}

bool TypeHelper::IsRelationalBinaryOPType(BinaryOPType type) {
  switch (type) {
    case BinaryOPType::kLarge:
    case BinaryOPType::kLargeEqual:
    case BinaryOPType::kEqual:
    case BinaryOPType::kLess:
    case BinaryOPType::kLessEqual:
    case BinaryOPType::kNotEqual:
      return true;
      break;
    default:
      return false;
  }
  return false;
}

bool TypeHelper::IsLogicalBinaryOPType(BinaryOPType type) {
  switch (type) {
    case BinaryOPType::kAnd:
    case BinaryOPType::kOr:
      return true;
      break;
    default:
      return false;
  }
  return false;
}

int TypeHelper::GetBitWidthFromType(ValueType type) {
  switch (type) {
    case ValueType::kI8:
      return 8;
    case ValueType::kI16:
      return 16;
    case ValueType::kI32:
      return 32;
    case ValueType::kI64:
      return 64;
    case ValueType::kU8:
      return 8;
    case ValueType::kU16:
      return 16;
    case ValueType::kU32:
      return 32;
    case ValueType::kU64:
      return 64;
    case ValueType::kF32:
      return 32;
    case ValueType::kF64:
      return 64;
    default:
      return -1;
  }
  return -1;
}

bool TypeHelper::IsSignedType(ValueType type) {
  switch (type) {
    case ValueType::kI8:
    case ValueType::kI16:
    case ValueType::kI32:
    case ValueType::kI64:
    case ValueType::kF32:
    case ValueType::kF64:
      return true;
    default:
      return false;
  }
  return false;
}

bool TypeHelper::IsFloatType(ValueType type) {
  switch (type) {
    case ValueType::kF32:
    case ValueType::kF64:
      return true;
    default:
      return false;
  }
}

bool TypeHelper::IsComplexType(ValueType type) {
  switch (type) {
    case ValueType::kString:
    case ValueType::kU8List:
    case ValueType::kU16List:
    case ValueType::kU32List:
    case ValueType::kU64List:
    case ValueType::kI8List:
    case ValueType::kI16List:
    case ValueType::kI32List:
    case ValueType::kI64List:
    case ValueType::kF32List:
    case ValueType::kF64List:
    case ValueType::kStringList:
      return true;
    default:
      return false;
  }
  return false;
}

}  // namespace jitfusion
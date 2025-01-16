/*
 * @Author: victorika
 * @Date: 2025-01-16 16:59:03
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-16 17:09:27
 */
#pragma once

#include <type.h>

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

}  // namespace jitfusion
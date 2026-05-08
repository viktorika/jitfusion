/*
 * @Author: victorika
 * @Date: 2026-05-07 19:00:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-05-07 19:00:00
 */
/*
 * TU-private helpers for invoking a JIT'd entry function and converting
 * its result into a variant RetType slot.
 *
 * Shared verbatim by src/exec_engine.cc and src/batch_exec_engine.cc.
 * Kept out of any public header because:
 *   * The helpers depend on <string>/<vector>/Status construction — not
 *     something we want to force onto every TU that merely includes
 *     exec_engine.h.
 *   * EXPAND_SWITCH_TYPE is a macro, not a real function; we do NOT want
 *     the identifier leaking to users. The two .cc files that include
 *     this header therefore also `#undef JF_EXPAND_SWITCH_TYPE` at the
 *     bottom of their TU (optional but hygienic).
 *
 * Why it is a macro rather than a templated function: every switch arm
 * needs a different concrete return type AND a different RetType variant
 * slot assignment. A function-based factoring would end up either
 * instantiating 23 specializations (O(n) template-bloat) or going
 * through an extra type-erasure indirection on the hot path. The macro
 * inlines the dispatch without cost and keeps all 23 arms in one place.
 */

#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "status.h"
#include "type.h"

namespace jitfusion::execute_dispatch {

using return_void_function_type = void (*)(void*, void*, void*);
using return_u8_function_type = std::uint8_t (*)(void*, void*, void*);
using return_u16_function_type = std::uint16_t (*)(void*, void*, void*);
using return_u32_function_type = std::uint32_t (*)(void*, void*, void*);
using return_u64_function_type = std::uint64_t (*)(void*, void*, void*);
using return_i8_function_type = std::int8_t (*)(void*, void*, void*);
using return_i16_function_type = std::int16_t (*)(void*, void*, void*);
using return_i32_function_type = std::int32_t (*)(void*, void*, void*);
using return_i64_function_type = std::int64_t (*)(void*, void*, void*);
using return_f32_function_type = float (*)(void*, void*, void*);
using return_f64_function_type = double (*)(void*, void*, void*);
using return_string_function_type = StringStruct (*)(void*, void*, void*);
using return_u8list_function_type = U8ListStruct (*)(void*, void*, void*);
using return_u16list_function_type = U16ListStruct (*)(void*, void*, void*);
using return_u32list_function_type = U32ListStruct (*)(void*, void*, void*);
using return_u64list_function_type = U64ListStruct (*)(void*, void*, void*);
using return_i8list_function_type = I8ListStruct (*)(void*, void*, void*);
using return_i16list_function_type = I16ListStruct (*)(void*, void*, void*);
using return_i32list_function_type = I32ListStruct (*)(void*, void*, void*);
using return_i64list_function_type = I64ListStruct (*)(void*, void*, void*);
using return_f32list_function_type = F32ListStruct (*)(void*, void*, void*);
using return_f64list_function_type = F64ListStruct (*)(void*, void*, void*);
using return_stringlist_function_type = StringListStruct (*)(void*, void*, void*);

}  // namespace jitfusion::execute_dispatch

// Shared dispatch: invokes the typed function pointer and writes the
// variant slot. Expects the caller to have `entry_arguments` (void*),
// `exec_ctx` (ExecContext, taken by address), and `result` (RetType*)
// in scope — same shape as the Execute / ExecuteAt overloads.
//
// The `return Status::RuntimeError(...)` in the default branch means
// the enclosing function must return Status, which all current callers
// do.
#define JF_EXPAND_SWITCH_TYPE(func_ptr, ret_type)                                                                      \
  switch (ret_type) {                                                                                                  \
    case ::jitfusion::ValueType::kVoid: {                                                                              \
      reinterpret_cast<::jitfusion::execute_dispatch::return_void_function_type>(func_ptr)(entry_arguments, &exec_ctx, \
                                                                                           nullptr);                   \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kU8: {                                                                                \
      *result = reinterpret_cast<::jitfusion::execute_dispatch::return_u8_function_type>(func_ptr)(                    \
          entry_arguments, &exec_ctx, nullptr);                                                                        \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kI8: {                                                                                \
      *result = reinterpret_cast<::jitfusion::execute_dispatch::return_i8_function_type>(func_ptr)(                    \
          entry_arguments, &exec_ctx, nullptr);                                                                        \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kU16: {                                                                               \
      *result = reinterpret_cast<::jitfusion::execute_dispatch::return_u16_function_type>(func_ptr)(                   \
          entry_arguments, &exec_ctx, nullptr);                                                                        \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kI16: {                                                                               \
      *result = reinterpret_cast<::jitfusion::execute_dispatch::return_i16_function_type>(func_ptr)(                   \
          entry_arguments, &exec_ctx, nullptr);                                                                        \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kU32: {                                                                               \
      *result = reinterpret_cast<::jitfusion::execute_dispatch::return_u32_function_type>(func_ptr)(                   \
          entry_arguments, &exec_ctx, nullptr);                                                                        \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kI32: {                                                                               \
      *result = reinterpret_cast<::jitfusion::execute_dispatch::return_i32_function_type>(func_ptr)(                   \
          entry_arguments, &exec_ctx, nullptr);                                                                        \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kU64: {                                                                               \
      *result = reinterpret_cast<::jitfusion::execute_dispatch::return_u64_function_type>(func_ptr)(                   \
          entry_arguments, &exec_ctx, nullptr);                                                                        \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kI64: {                                                                               \
      *result = reinterpret_cast<::jitfusion::execute_dispatch::return_i64_function_type>(func_ptr)(                   \
          entry_arguments, &exec_ctx, nullptr);                                                                        \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kF32: {                                                                               \
      *result = reinterpret_cast<::jitfusion::execute_dispatch::return_f32_function_type>(func_ptr)(                   \
          entry_arguments, &exec_ctx, nullptr);                                                                        \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kF64: {                                                                               \
      *result = reinterpret_cast<::jitfusion::execute_dispatch::return_f64_function_type>(func_ptr)(                   \
          entry_arguments, &exec_ctx, nullptr);                                                                        \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kString: {                                                                            \
      ::jitfusion::StringStruct string = reinterpret_cast<::jitfusion::execute_dispatch::return_string_function_type>( \
          func_ptr)(entry_arguments, &exec_ctx, nullptr);                                                              \
      std::string res(string.data, string.len);                                                                        \
      *result = std::move(res);                                                                                        \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kU8List: {                                                                            \
      ::jitfusion::U8ListStruct list = reinterpret_cast<::jitfusion::execute_dispatch::return_u8list_function_type>(   \
          func_ptr)(entry_arguments, &exec_ctx, nullptr);                                                              \
      *result = std::vector<std::uint8_t>(list.data, list.data + list.len);                                            \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kU16List: {                                                                           \
      ::jitfusion::U16ListStruct list = reinterpret_cast<::jitfusion::execute_dispatch::return_u16list_function_type>( \
          func_ptr)(entry_arguments, &exec_ctx, nullptr);                                                              \
      *result = std::vector<std::uint16_t>(list.data, list.data + list.len);                                           \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kU32List: {                                                                           \
      ::jitfusion::U32ListStruct list = reinterpret_cast<::jitfusion::execute_dispatch::return_u32list_function_type>( \
          func_ptr)(entry_arguments, &exec_ctx, nullptr);                                                              \
      *result = std::vector<std::uint32_t>(list.data, list.data + list.len);                                           \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kU64List: {                                                                           \
      ::jitfusion::U64ListStruct list = reinterpret_cast<::jitfusion::execute_dispatch::return_u64list_function_type>( \
          func_ptr)(entry_arguments, &exec_ctx, nullptr);                                                              \
      *result = std::vector<std::uint64_t>(list.data, list.data + list.len);                                           \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kI8List: {                                                                            \
      ::jitfusion::I8ListStruct list = reinterpret_cast<::jitfusion::execute_dispatch::return_i8list_function_type>(   \
          func_ptr)(entry_arguments, &exec_ctx, nullptr);                                                              \
      *result = std::vector<std::int8_t>(list.data, list.data + list.len);                                             \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kI16List: {                                                                           \
      ::jitfusion::I16ListStruct list = reinterpret_cast<::jitfusion::execute_dispatch::return_i16list_function_type>( \
          func_ptr)(entry_arguments, &exec_ctx, nullptr);                                                              \
      *result = std::vector<std::int16_t>(list.data, list.data + list.len);                                            \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kI32List: {                                                                           \
      ::jitfusion::I32ListStruct list = reinterpret_cast<::jitfusion::execute_dispatch::return_i32list_function_type>( \
          func_ptr)(entry_arguments, &exec_ctx, nullptr);                                                              \
      *result = std::vector<std::int32_t>(list.data, list.data + list.len);                                            \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kI64List: {                                                                           \
      ::jitfusion::I64ListStruct list = reinterpret_cast<::jitfusion::execute_dispatch::return_i64list_function_type>( \
          func_ptr)(entry_arguments, &exec_ctx, nullptr);                                                              \
      *result = std::vector<std::int64_t>(list.data, list.data + list.len);                                            \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kF32List: {                                                                           \
      ::jitfusion::F32ListStruct list = reinterpret_cast<::jitfusion::execute_dispatch::return_f32list_function_type>( \
          func_ptr)(entry_arguments, &exec_ctx, nullptr);                                                              \
      *result = std::vector<float>(list.data, list.data + list.len);                                                   \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kF64List: {                                                                           \
      ::jitfusion::F64ListStruct list = reinterpret_cast<::jitfusion::execute_dispatch::return_f64list_function_type>( \
          func_ptr)(entry_arguments, &exec_ctx, nullptr);                                                              \
      *result = std::vector<double>(list.data, list.data + list.len);                                                  \
    } break;                                                                                                           \
    case ::jitfusion::ValueType::kStringList: {                                                                        \
      ::jitfusion::StringListStruct list =                                                                             \
          reinterpret_cast<::jitfusion::execute_dispatch::return_stringlist_function_type>(func_ptr)(                  \
              entry_arguments, &exec_ctx, nullptr);                                                                    \
      std::vector<std::string> res;                                                                                    \
      res.reserve(list.len);                                                                                           \
      for (std::uint32_t i = 0; i < list.len; ++i) {                                                                   \
        res.emplace_back(list.data[i].data, list.data[i].len);                                                         \
      }                                                                                                                \
      *result = std::move(res);                                                                                        \
    } break;                                                                                                           \
    default:                                                                                                           \
      return ::jitfusion::Status::RuntimeError("Unsupported return type");                                             \
  }

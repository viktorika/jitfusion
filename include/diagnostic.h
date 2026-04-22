/*
 * @Author: victorika
 * @Date: 2026-04-21 10:30:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-21 10:30:00
 */
#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "exec_node.h"

namespace jitfusion {

// Diagnostic composes a user-facing error message with an optional source
// location and extra context (notes / hints). When rendered against the
// original source code it produces a Rust/Clang-style message, e.g.
//
//   error: if branches have incompatible types
//     --> line 3, column 12
//      |
//    3 |   x = if(a > 0, 1, "hi");
//      |            ^^^^^^^^^^^^^
//      = note: then branch: i32
//      = note: else branch: string
//      = hint: make both branches the same type
//
// If `loc` is invalid (i.e. the AST was built via the C++ API rather than the
// Athena parser), Render falls back to a plain `level: message` line with any
// notes/hints appended.
class Diagnostic {
 public:
  enum class Level : uint8_t { kError, kWarning };

  Diagnostic(Level level, std::string message, SourceLocation loc)
      : level_(level), message_(std::move(message)), loc_(loc) {}

  Diagnostic& WithNote(std::string note) {
    notes_.emplace_back(std::move(note));
    return *this;
  }
  Diagnostic& WithHint(std::string hint) {
    hints_.emplace_back(std::move(hint));
    return *this;
  }

  [[nodiscard]] std::string Render() const;

  [[nodiscard]] const SourceLocation& GetLocation() const { return loc_; }
  [[nodiscard]] const std::string& GetMessage() const { return message_; }

 private:
  Level level_;
  std::string message_;
  SourceLocation loc_;
  std::vector<std::string> notes_;
  std::vector<std::string> hints_;
};

}  // namespace jitfusion

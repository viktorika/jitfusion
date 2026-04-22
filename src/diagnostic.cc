/*
 * @Author: victorika
 * @Date: 2026-04-21 10:30:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-21 10:30:00
 */
#include "diagnostic.h"

#include <algorithm>
#include <sstream>
#include <vector>

namespace jitfusion {

namespace {

constexpr const char* LevelToString(Diagnostic::Level level) {
  switch (level) {
    case Diagnostic::Level::kError:
      return "error";
    case Diagnostic::Level::kWarning:
      return "warning";
  }
  return "error";
}

// Split source code into lines, keeping empty trailing lines. Line numbers
// used by Bison start at 1, so callers should index with (line - 1).
std::vector<std::string_view> SplitLines(std::string_view text) {
  std::vector<std::string_view> lines;
  size_t start = 0;
  for (size_t i = 0; i < text.size(); ++i) {
    if (text[i] == '\n') {
      lines.emplace_back(text.substr(start, i - start));
      start = i + 1;
    }
  }
  if (start <= text.size()) {
    lines.emplace_back(text.substr(start));
  }
  return lines;
}

// Pad a line number string to `width` spaces on the left.
std::string PadLeft(const std::string& s, size_t width) {
  if (s.size() >= width) {
    return s;
  }
  return std::string(width - s.size(), ' ') + s;
}

}  // namespace

std::string Diagnostic::Render() const {
  std::ostringstream os;
  os << LevelToString(level_) << ": " << message_;

  const std::string_view source_code = loc_.source_code;
  if (!loc_.Valid() || source_code.empty()) {
    // No location available — just append notes and hints and return.
    for (const auto& note : notes_) {
      os << "\n  = note: " << note;
    }
    for (const auto& hint : hints_) {
      os << "\n  = hint: " << hint;
    }
    return os.str();
  }

  os << "\n  --> line " << loc_.begin_line << ", column " << loc_.begin_col;

  auto lines = SplitLines(source_code);
  const int total_lines = static_cast<int>(lines.size());
  // Gutter width is based on the largest line number we might print.
  const std::string max_ln = std::to_string(std::min(loc_.end_line, total_lines));
  const size_t gutter = max_ln.size();
  const std::string empty_gutter(gutter, ' ');

  // Blank separator row.
  os << "\n " << empty_gutter << " |";

  // Print the offending lines with a caret underline on each.
  const int start_line = std::max(1, loc_.begin_line);
  const int end_line = std::max(loc_.end_line, loc_.begin_line);
  for (int ln = start_line; ln <= end_line && ln <= total_lines; ++ln) {
    std::string_view src_line = lines[ln - 1];
    os << "\n " << PadLeft(std::to_string(ln), gutter) << " | " << src_line;

    // Determine caret range on this specific line.
    // Bison columns are 1-based and point at the character position.
    int caret_begin = 1;
    int caret_end = static_cast<int>(src_line.size()) + 1;
    if (ln == loc_.begin_line) {
      caret_begin = std::max(1, loc_.begin_col);
    }
    if (ln == loc_.end_line) {
      caret_end = std::max(caret_begin + 1, loc_.end_col);
    }
    int caret_len = caret_end - caret_begin;
    caret_len = std::max(caret_len, 1);

    os << "\n " << empty_gutter << " | " << std::string(caret_begin - 1, ' ')
       << std::string(static_cast<size_t>(caret_len), '^');
  }

  for (const auto& note : notes_) {
    os << "\n " << empty_gutter << " = note: " << note;
  }
  for (const auto& hint : hints_) {
    os << "\n " << empty_gutter << " = hint: " << hint;
  }
  return os.str();
}

}  // namespace jitfusion

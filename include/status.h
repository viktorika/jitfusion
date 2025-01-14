/*
 * @Author: victorika
 * @Date: 2025-01-14 15:41:47
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-14 16:07:54
 */
#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <utility>

namespace jitfusion {

class Status {
 public:
  Status() = default;
  ~Status() = default;
  Status(const Status& rhs) = delete;
  Status& operator=(const Status& rhs) = delete;
  Status(Status&& rhs) = default;
  Status& operator=(Status&& rhs) = default;

  static Status OK() { return {}; }

  [[nodiscard]] bool ok() const { return (state_ == nullptr || state_->code_ == StatusCode::kOk); }

  [[nodiscard]] std::string ToString() const;

 private:
  enum class StatusCode : uint8_t {
    kOk = 0,
  };
  template <typename... Args>
  explicit Status(StatusCode code, Args&&... args) {
    std::stringstream ss;
    (ss << ... << std::forward<Args>(args));
    state_ = std::make_unique<State>();
    state_->code_ = code;
    state_->msg_ = ss.str();
  }

  struct State {
    StatusCode code_{StatusCode::kOk};
    std::string msg_;
  };
  std::unique_ptr<State> state_;
};

#define RE_RETURN_NOT_OK(expr) \
  do {                         \
    Status st = (expr);        \
    if (!st.ok()) {            \
      return st;               \
    }                          \
  } while (false)
}  // namespace jitfusion

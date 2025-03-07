/*
 * @Author: victorika
 * @Date: 2025-01-14 15:41:47
 * @Last Modified by: victorika
 * @Last Modified time: 2025-01-16 17:06:48
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

  template <typename... Args>
  static Status InvalidArgument(Args&&... args) {
    return Status(Status::StatusCode::kInvalidArgument, std::forward<Args>(args)...);
  }
  template <typename... Args>
  static Status RuntimeError(Args&&... args) {
    return Status(Status::StatusCode::kRuntimeError, std::forward<Args>(args)...);
  }
  template <typename... Args>
  static Status ParseError(Args&&... args) {
    return Status(Status::StatusCode::kParseError, std::forward<Args>(args)...);
  }
  template <typename... Args>
  static Status NotImplemented(Args&&... args) {
    return Status(Status::StatusCode::kNotImplemented, std::forward<Args>(args)...);
  }

  [[nodiscard]] bool ok() const { return (state_ == nullptr || state_->code_ == StatusCode::kOk); }

  [[nodiscard]] std::string ToString() const {
    if (state_ == nullptr) {
      return "OK";
    }
    std::string result(ErrName(ok() ? StatusCode::kOk : state_->code_));
    result += ": ";
    result += state_->msg_;
    return result;
  }

 private:
  enum class StatusCode : uint8_t {
    kOk = 0,
    kInvalidArgument = 1,
    kRuntimeError = 2,
    kParseError = 3,
    kNotImplemented = 4,
  };
  static std::string ErrName(StatusCode code) {
    switch (code) {
      case StatusCode::kOk:
        return "OK";
      case StatusCode::kRuntimeError:
        return "Runtime Error";
      case StatusCode::kInvalidArgument:
        return "Argument Invalid";
      case StatusCode::kNotImplemented:
        return "NotImplemented";
      case StatusCode::kParseError:
        return "Parse Error";
      default:
        return "Unknown Code";
    }
  }

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

#define JF_RETURN_NOT_OK(expr) \
  do {                      \
    Status st = (expr);     \
    if (!st.ok()) {         \
      return st;            \
    }                       \
  } while (false)
}  // namespace jitfusion

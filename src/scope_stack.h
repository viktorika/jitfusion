/*
 * @Author: victorika
 * @Date: 2026-04-17 14:30:00
 * @Last Modified by: victorika
 * @Last Modified time: 2026-04-17 16:00:00
 */
#pragma once

#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace jitfusion {

template <typename V>
class ScopeStack {
 public:
  ScopeStack() : stack_(1) {}

  void PushScope() { stack_.emplace_back(); }
  void PopScope() { stack_.pop_back(); }

  void Set(const std::string& name, V value) { stack_.back()[name] = std::move(value); }

  void SetOrUpdate(const std::string& name, V value) {
    for (auto it = stack_.rbegin(); it != stack_.rend(); ++it) {
      if (auto found = it->find(name); found != it->end()) {
        found->second = std::move(value);
        return;
      }
    }
    stack_.back()[name] = std::move(value);
  }

  V Lookup(const std::string& name) const {
    for (auto it = stack_.rbegin(); it != stack_.rend(); ++it) {
      if (auto found = it->find(name); found != it->end()) {
        return found->second;
      }
    }
    if constexpr (std::is_pointer_v<V>) {
      return nullptr;
    } else {
      return V{};
    }
  }

  std::unordered_map<std::string, V> Snapshot() const {
    std::unordered_map<std::string, V> result;
    for (const auto& scope : stack_) {
      for (const auto& [name, value] : scope) {
        result[name] = value;
      }
    }
    return result;
  }

  std::unordered_map<std::string, V> GetShadowed() const {
    std::unordered_map<std::string, V> shadowed;
    for (const auto& [name, value] : stack_.back()) {
      for (auto it = stack_.rbegin() + 1; it != stack_.rend(); ++it) {
        if (it->find(name) != it->end()) {
          shadowed[name] = value;
          break;
        }
      }
    }
    return shadowed;
  }

 private:
  std::vector<std::unordered_map<std::string, V>> stack_;
};

}  // namespace jitfusion

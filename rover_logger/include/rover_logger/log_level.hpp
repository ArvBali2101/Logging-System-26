#pragma once
#include <string_view>

namespace rover_logger {

// A scoped, ordered set of severities.
// The explicit integer values make comparisons trivial and stable.
enum class LogLevel : int {
  TRACE = 0,
  DEBUG = 1,
  INFO = 2,
  WARN = 3,
  ERROR = 4,
  FATAL = 5,
};

// Allocation-free, header-only name lookup for fast printing/formatting.
// constexpr => can be evaluated at compile-time and inlined aggressively.
constexpr std::string_view to_string(LogLevel lv) {
  switch (lv) {
    case LogLevel::TRACE:
      return "TRACE";
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARN:
      return "WARN";
    case LogLevel::ERROR:
      return "ERROR";
    case LogLevel::FATAL:
      return "FATAL";
  }
  return "?";  // Defensive default: should never hit if enum is exhaustive.
}

}  // namespace rover_logger
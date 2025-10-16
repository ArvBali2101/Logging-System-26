#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include "rover_logger/log_level.hpp"

using namespace rover_logger;

int main() {
  // 1) Ordering invariants: TRACE < DEBUG < ... < FATAL
  static_assert(static_cast<int>(LogLevel::TRACE) == 0);
  static_assert(static_cast<int>(LogLevel::FATAL) == 5);
  assert(static_cast<int>(LogLevel::DEBUG) > static_cast<int>(LogLevel::TRACE));
  assert(static_cast<int>(LogLevel::ERROR) > static_cast<int>(LogLevel::WARN));

  // 2) Names are stable and allocation-free
  assert(to_string(LogLevel::INFO)  == std::string_view{"INFO"});
  assert(to_string(LogLevel::ERROR) == std::string_view{"ERROR"});

  // 3) Sorting by severity uses integer compare semantics
  std::vector<LogLevel> v = {
    LogLevel::ERROR, LogLevel::TRACE, LogLevel::WARN, LogLevel::INFO
  };
  std::sort(v.begin(), v.end(),
            [](LogLevel a, LogLevel b){ return static_cast<int>(a) < static_cast<int>(b); });
  assert(v.front() == LogLevel::TRACE);
  assert(v.back()  == LogLevel::ERROR);

  // 4) Print a human-friendly demo line (for visual confirmation)
  std::cout << "LogLevel OK: "
            << to_string(LogLevel::TRACE) << ", "
            << to_string(LogLevel::DEBUG) << ", "
            << to_string(LogLevel::INFO)  << ", "
            << to_string(LogLevel::WARN)  << ", "
            << to_string(LogLevel::ERROR) << ", "
            << to_string(LogLevel::FATAL) << "\n";

  std::cout << "OK: test_log_level passed.\n";
  return 0;
}

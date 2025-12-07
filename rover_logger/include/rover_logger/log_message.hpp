#pragma once
#include <chrono>
#include <string>
#include <utility>

#include "rover_logger/log_level.hpp"

namespace rover_logger {

// Immutable log payload that all sinks see.
struct LogMessage {
  using clock = std::chrono::system_clock;

  LogLevel level;
  std::string module;  // e.g. "/drive", "/vision"
  std::string text;    // formatted log text
  clock::time_point ts;

  LogMessage(LogLevel lvl, std::string mod, std::string msg)
      : level(lvl),
        module(std::move(mod)),
        text(std::move(msg)),
        ts(clock::now()) {}
};

}  // namespace rover_logger

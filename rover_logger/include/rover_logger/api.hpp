#pragma once
#include <string>

#include "rover_logger/log_level.hpp"
#include "rover_logger/logger.hpp"

namespace rover_logger {

// Core printf-style helper.
// Example:
//   log_printf(logger, LogLevel::INFO, "/drive", "Speed=%d", speed);
void log_printf(Logger& logger, LogLevel level, const std::string& module,
                const char* fmt, ...);

}  // namespace rover_logger

// Public one-line macros – *this* is what subsystems will use.
// These satisfy "one-line interface", "per-log config", and
// "printf-like formatting" requirements.

#define RVLOG_TRACE(logger, module, ...)                                \
  ::rover_logger::log_printf((logger), ::rover_logger::LogLevel::TRACE, \
                             (module), __VA_ARGS__)

#define RVLOG_DEBUG(logger, module, ...)                                \
  ::rover_logger::log_printf((logger), ::rover_logger::LogLevel::DEBUG, \
                             (module), __VA_ARGS__)

#define RVLOG_INFO(logger, module, ...)                                \
  ::rover_logger::log_printf((logger), ::rover_logger::LogLevel::INFO, \
                             (module), __VA_ARGS__)

#define RVLOG_WARN(logger, module, ...)                                \
  ::rover_logger::log_printf((logger), ::rover_logger::LogLevel::WARN, \
                             (module), __VA_ARGS__)

#define RVLOG_ERROR(logger, module, ...)                                \
  ::rover_logger::log_printf((logger), ::rover_logger::LogLevel::ERROR, \
                             (module), __VA_ARGS__)

#define RVLOG_FATAL(logger, module, ...)                                \
  ::rover_logger::log_printf((logger), ::rover_logger::LogLevel::FATAL, \
                             (module), __VA_ARGS__)

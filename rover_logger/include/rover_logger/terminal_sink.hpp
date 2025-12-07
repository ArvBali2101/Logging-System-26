#pragma once

#include <mutex>
#include <string>

#include "rover_logger/log_level.hpp"
#include "rover_logger/log_message.hpp"
#include "rover_logger/logger.hpp"

namespace rover_logger {

// Writes human-readable logs to stdout with optional ANSI colours.
class TerminalSink : public ILogSink {
 public:
  explicit TerminalSink(bool colorize);

  // Core logging hook used by Logger
  void write(const LogMessage& msg) override;
  void flush() override {}

 private:
  static const char* color_for(LogLevel lv);
  static std::string format_ts(const LogMessage::clock::time_point& tp);

  bool colorize_;
  std::mutex m_;
};

}  // namespace rover_logger

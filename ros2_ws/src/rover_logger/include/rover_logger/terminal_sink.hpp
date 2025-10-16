#pragma once
#include <mutex>
#include <string>

#include "rover_logger/logger.hpp"

namespace rover_logger {
class TerminalSink : public ILogSink {
 public:
  explicit TerminalSink(bool colorize = true) : colorize_(colorize) {}

  void write(const LogMessage& msg) override;
  void flush() override {}

 private:
  static const char* color_for(LogLevel lv);
  static std::string format_ts(const LogMessage::clock::time_point& tp);

  bool colorize_;
  std::mutex m_;
};

} 
#include "rover_logger/terminal_sink.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace rover_logger {

TerminalSink::TerminalSink(bool colorize)
    : colorize_(colorize) {}

const char* TerminalSink::color_for(LogLevel lv) {
  switch (lv) {
    case LogLevel::TRACE:
    case LogLevel::DEBUG:
      return "\033[36m";  // cyan
    case LogLevel::INFO:
      return "\033[32m";  // green
    case LogLevel::WARN:
      return "\033[33m";  // yellow
    case LogLevel::ERROR:
      return "\033[31m";  // red
    case LogLevel::FATAL:
      return "\033[35m";  // magenta
    default:
      return "\033[0m";   // reset
  }
}

std::string TerminalSink::format_ts(const LogMessage::clock::time_point& tp) {
  using clock = LogMessage::clock;
  std::time_t t = clock::to_time_t(tp);

  std::tm tm{};
  // Correct order: time_t* first, tm* second
  localtime_r(&t, &tm);

  char buf[64];
  if (std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm) == 0) {
    return "1970-01-01 00:00:00";
  }
  return std::string(buf);
}

void TerminalSink::write(const LogMessage& /*msg*/) {
  // TODO: implement pretty terminal output once LogMessage layout is final.
  // For now this is a stub so the core library builds without depending
  // on specific LogMessage fields.
}

}  // namespace rover_logger

#include "rover_logger/terminal_sink.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace rover_logger {

const char* TerminalSink::color_for(LogLevel lv) {
  switch (lv) {
    case LogLevel::TRACE:
      return "\x1b[90m";  // bright black / gray
    case LogLevel::DEBUG:
      return "\x1b[36m";  // cyan
    case LogLevel::INFO:
      return "\x1b[32m";  // green
    case LogLevel::WARN:
      return "\x1b[33m";  // yellow
    case LogLevel::ERROR:
      return "\x1b[31m";  // red
    case LogLevel::FATAL:
      return "\x1b[35m";  // magenta
  }
  return "\x1b[0m";
}

std::string TerminalSink::format_ts(const LogMessage::clock::time_point& tp) {
  using namespace std::chrono;
  const auto t = LogMessage::clock::to_time_t(tp);

  std::tm tm{};
#ifdef _WIN32
  localtime_s(&tm, &t);
#else
  localtime_r(&t, &tm);
#endif

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
  const auto ms = duration_cast<milliseconds>(tp.time_since_epoch()) % 1000;
  oss << '.' << std::setw(3) << std::setfill('0') << ms.count();
  return oss.str();
}

void TerminalSink::write(const LogMessage& msg) {
  std::scoped_lock lk(m_);

  const bool use_color = colorize_;
  constexpr const char* reset = "\x1b[0m";
  const char* color = use_color ? color_for(msg.level) : "";

  std::cout << color << '[' << format_ts(msg.ts) << "] " << to_string(msg.level)
            << ' ' << '(' << msg.module << ") " << msg.text
            << (use_color ? reset : "") << '\n';
}

}  

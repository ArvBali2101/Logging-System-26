#include "rover_logger/json_formatter.hpp"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace rover_logger {

std::string json_escape(std::string_view s) {
  std::string out;
  out.reserve(s.size() + 8);
  for (unsigned char c : s) {
    switch (c) {
      case '\"':
        out += "\\\"";
        break;
      case '\\':
        out += "\\\\";
        break;
      case '\b':
        out += "\\b";
        break;
      case '\f':
        out += "\\f";
        break;
      case '\n':
        out += "\\n";
        break;
      case '\r':
        out += "\\r";
        break;
      case '\t':
        out += "\\t";
        break;
      default:
        if (c < 0x20) {
          char buf[7];
          std::snprintf(buf, sizeof(buf), "\\u%04X", c);
          out += buf;
        } else {
          out.push_back(static_cast<char>(c));
        }
    }
  }
  return out;
}

std::string iso8601_utc_ms(const LogMessage::clock::time_point& tp) {
  using namespace std::chrono;
  auto t = LogMessage::clock::to_time_t(tp);
  std::tm tm{};
#if defined(_WIN32)
  gmtime_s(&tm, &t);
#else
  gmtime_r(&t, &tm);
#endif
  auto ms = duration_cast<milliseconds>(tp.time_since_epoch()) % 1000;

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S") << '.' << std::setw(3)
      << std::setfill('0') << ms.count() << 'Z';
  return oss.str();
}

std::string to_json_line(const LogMessage& msg) {
  std::string j;
  j.reserve(64 + msg.module.size() + msg.text.size());
  j += "{\"ts\":\"";
  j += iso8601_utc_ms(msg.ts);
  j += "\",\"level\":\"";
  j += std::string(to_string(msg.level));
  j += "\",\"module\":\"";
  j += json_escape(msg.module);
  j += "\",\"message\":\"";
  j += json_escape(msg.text);
  j += "\"}";
  return j;
}

}  // namespace rover_logger

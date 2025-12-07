#include "rover_logger/api.hpp"

#include <cstdarg>
#include <cstdio>
#include <vector>

#include "rover_logger/log_message.hpp"

namespace rover_logger {

// Internal printf formatter with stack buffer + fallback.
static std::string vformat(const char* fmt, va_list args) {
  char buf[256];

  va_list copy;
  va_copy(copy, args);
  int needed = std::vsnprintf(buf, sizeof(buf), fmt, copy);
  va_end(copy);

  if (needed < 0) {
    return "log formatting error";
  }
  if (needed < static_cast<int>(sizeof(buf))) {
    return std::string(buf, static_cast<std::size_t>(needed));
  }

  std::vector<char> dyn(static_cast<std::size_t>(needed) + 1);
  std::vsnprintf(dyn.data(), dyn.size(), fmt, args);
  return std::string(dyn.data(), static_cast<std::size_t>(needed));
}

void log_printf(Logger& logger,
                LogLevel level,
                const std::string& module,
                const char* fmt,
                ...) {
  va_list args;
  va_start(args, fmt);
  std::string text = vformat(fmt, args);
  va_end(args);

  LogMessage msg{level, module, std::move(text)};
  logger.log(std::move(msg));
}

}  // namespace rover_logger

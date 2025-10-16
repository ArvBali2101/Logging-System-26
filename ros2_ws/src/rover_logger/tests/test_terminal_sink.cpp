#include <iostream>
#include "rover_logger/logger.hpp"
#include "rover_logger/terminal_sink.hpp"

using namespace rover_logger;

int main() {
  Logger log(64);
  auto term = std::make_shared<TerminalSink>(true);
  log.add_sink(term);
  log.set_min_level(LogLevel::TRACE);

  log.log(LogMessage{LogLevel::TRACE, "demo", "trace msg"});
  log.log(LogMessage{LogLevel::DEBUG, "demo", "debug msg"});
  log.log(LogMessage{LogLevel::INFO,  "demo", "info msg"});
  log.log(LogMessage{LogLevel::WARN,  "demo", "warn msg"});
  log.log(LogMessage{LogLevel::ERROR, "demo", "error msg"});
  log.log(LogMessage{LogLevel::FATAL, "demo", "fatal msg"});

  // give worker a moment
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  std::cout << "OK: test_terminal_sink passed.\n";
  return 0;
}

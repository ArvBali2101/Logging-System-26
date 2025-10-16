#include <cassert>
#include <iostream>
#include <thread>
#include "rover_logger/log_message.hpp"

using namespace rover_logger;

int main() {
  // Construct a log message
  LogMessage msg{LogLevel::INFO, "demo.module", "hello committee"};

  // Assert fields are populated correctly
  assert(msg.level == LogLevel::INFO);
  assert(msg.module == "demo.module");
  assert(msg.text == "hello committee");

  // Timestamp sanity: should not be in the future
  auto now = LogMessage::clock::now();
  assert(msg.ts <= now);

  // Visual check: print
  std::cout << "[" << to_string(msg.level) << "]"
            << " (" << msg.module << ") "
            << msg.text << "\n";

  std::cout << "OK: test_log_message passed.\n";
  return 0;
}

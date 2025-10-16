#include <cassert>
#include <iostream>
#include <string>
#include "rover_logger/json_formatter.hpp"

using namespace rover_logger;

// Utility: check that substrings appear in order (by position)
static bool appears_in_order(const std::string& s,
                             const std::string& a,
                             const std::string& b,
                             const std::string& c,
                             const std::string& d) {
  auto pa = s.find(a);
  auto pb = s.find(b);
  auto pc = s.find(c);
  auto pd = s.find(d);
  return pa != std::string::npos && pb != std::string::npos &&
         pc != std::string::npos && pd != std::string::npos &&
         pa < pb && pb < pc && pc < pd;
}

int main() {
  // 1) Basic schema shape and key order
  LogMessage m{LogLevel::INFO, "drive.motor", "target=2.0A"};
  const std::string js = to_json_line(m);
  // Keys must appear in this strict order:
  assert(appears_in_order(js, "\"ts\"", "\"level\"", "\"module\"", "\"message\""));

  // 2) Level string correctness
  assert(js.find("\"level\":\"INFO\"") != std::string::npos);

  // 3) Timestamp format sanity: ends with 'Z' and has milliseconds ".XYZZ"
  // Rough check: 'T' present and 'Z' at end
  assert(js.find('T') != std::string::npos);
  assert(js.back() == '}');
  {
    // Extract the ts value to check trailing Z
    auto start = js.find("\"ts\":\"");
    assert(start != std::string::npos);
    start += 6;
    auto endq = js.find('"', start);
    assert(endq != std::string::npos);
    std::string ts = js.substr(start, endq - start);
    assert(!ts.empty() && ts.back() == 'Z');
  }

  // 4) Escaping: quotes and control chars are escaped
  LogMessage e{LogLevel::WARN, "mod\"x\\y", "bad\nchars\t\"here\""};
  const std::string je = to_json_line(e);
  assert(je.find("\\\"") != std::string::npos); // escaped quote
  assert(je.find("\\\\") != std::string::npos); // escaped backslash
  assert(je.find("\\n")  != std::string::npos); // escaped newline
  assert(je.find("\\t")  != std::string::npos); // escaped tab

  // 5) Line is single-line (no raw newlines)
  assert(je.find('\n') == std::string::npos);

  std::cout << "OK: test_json_formatter passed.\n";
  return 0;
}
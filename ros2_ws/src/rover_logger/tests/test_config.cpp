#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include "rover_logger/config.hpp"

using namespace rover_logger;

static std::string write_temp_yaml(const char* content) {
  const std::string path = "build/tmp_config.yaml";
  std::ofstream f(path);
  f << content;
  f.close();
  return path;
}

int main() {
  const char* YAML_TEXT = R"YAML(
level: warn
max_queue: 2048
sinks:
  - type: terminal
    colorize: false
  - type: file
    path: "/var/log/rover/rover.log"
    rotation_bytes: 1048576
    rotate_keep: 5
    compress: true
modules:
  /drive: error
  /vision: info
)YAML";

  // Write YAML to a temp file
  const std::string path = write_temp_yaml(YAML_TEXT);

  // Load and validate
  LoggerConfig cfg = load_config_file(path);

  // Global
  assert(cfg.level == LogLevel::WARN);
  assert(cfg.max_queue == 2048);

  // Sinks
  assert(cfg.sinks.size() == 2);
  // terminal
  assert(cfg.sinks[0].type == "terminal");
  assert(cfg.sinks[0].colorize.has_value() && cfg.sinks[0].colorize.value() == false);
  // file
  assert(cfg.sinks[1].type == "file");
  assert(cfg.sinks[1].path.has_value() && cfg.sinks[1].path.value() == "/var/log/rover/rover.log");
  assert(cfg.sinks[1].rotation_bytes.has_value() && cfg.sinks[1].rotation_bytes.value() == 1048576u);
  assert(cfg.sinks[1].rotate_keep.has_value() && cfg.sinks[1].rotate_keep.value() == 5);
  assert(cfg.sinks[1].compress.has_value() && cfg.sinks[1].compress.value() == true);

  // Modules
  assert(cfg.modules.size() == 2);
  assert(cfg.modules.at("/drive") == LogLevel::ERROR);
  assert(cfg.modules.at("/vision") == LogLevel::INFO);

  // parse_level edge cases
  try {
    (void)parse_level("warning"); // accepted alias
  } catch (...) { assert(false && "warning alias should be accepted"); }
  try {
    (void)parse_level("unknown");
    assert(false && "unknown level should throw");
  } catch (const std::invalid_argument&) {
    // expected
  }

  std::cout << "OK: test_config passed.\n";
  return 0;
}
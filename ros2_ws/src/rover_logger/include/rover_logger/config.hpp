#pragma once
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "rover_logger/log_level.hpp"

namespace rover_logger {

struct SinkConfig {
  std::string type;
  std::optional<bool> colorize;
  std::optional<std::string> path;
  std::optional<std::size_t> rotation_bytes;
  std::optional<int> rotate_keep;
  std::optional<bool> compress;
  std::optional<std::string> host;
  std::optional<int> port;
};
struct LoggerConfig {
  LogLevel level = LogLevel::INFO;
  std::size_t max_queue = 4096;
  std::vector<SinkConfig> sinks;
  std::unordered_map<std::string, LogLevel> modules;
};
LogLevel parse_level(std::string_view s);

LoggerConfig load_config_file(const std::string& path);

} 

#pragma once

// Standard headers for handling basic types, optional values, strings,
// string views (non-owning string references), hash maps, and arrays.
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

// Defines the LogLevel enum (TRACE, DEBUG, INFO, WARN, ERROR, FATAL).
// This header must be included so LoggerConfig can store per-module levels.
#include "log_level.hpp"

namespace rover_logger {

// ---------------------------------------------------------------------------
// SinkConfig
// ---------------------------------------------------------------------------
// Represents the configuration for a single "sink"—a destination where log
// messages are written. Each sink may be of a different type:
//
// Examples:
//   - Terminal sink (prints to console)
//   - File sink (writes to rotating log files)
//   - Network sink (TCP/UDP log streaming)
// Each field is optional because different sink types require different fields.
// ---------------------------------------------------------------------------
struct SinkConfig {
  std::string type;                        // e.g., "terminal", "file", "tcp"

  std::optional<bool> colorize;            // For terminal sinks: enable colors
  std::optional<std::string> path;         // For file sinks: base filename
  std::optional<std::size_t> rotation_bytes; // File size before rotation
  std::optional<int> rotate_keep;          // Number of old rotated files to keep
  std::optional<bool> compress;            // Compress rotated logs if true

  std::optional<std::string> host;         // For network sinks: server address
  std::optional<int> port;                 // For network sinks: server port
};

// ---------------------------------------------------------------------------
// LoggerConfig
// ---------------------------------------------------------------------------
// Represents the full configuration for the logging system. This is loaded
// from a YAML configuration file (e.g. logger.yaml).
//
// Fields:
//   - level: Global minimum log level (e.g., INFO). Modules can override.
//   - max_queue: Size of the internal async logging queue.
//   - sinks: List of all sinks (console, file, network).
//   - modules: Per-module log level overrides.
//     Example:
//         modules["/nav"] = LogLevel::DEBUG;
//
// This lets each subsystem control its logging verbosity independently.
// ---------------------------------------------------------------------------
struct LoggerConfig {
  LogLevel level = LogLevel::INFO;                 // Default global log level
  std::size_t max_queue = 4096;                    // Max async queue size
  std::vector<SinkConfig> sinks;                   // List of output sinks
  std::unordered_map<std::string, LogLevel> modules; // Per-module log levels
};

// ---------------------------------------------------------------------------
// parse_level
// ---------------------------------------------------------------------------
// Converts a string like "info", "warn", or "debug" into a LogLevel enum.
//
// Called by the YAML config loader to interpret human-readable log levels.
// Throws an error if the string does not correspond to a valid log level.
// ---------------------------------------------------------------------------
LogLevel parse_level(std::string_view s);

// ---------------------------------------------------------------------------
// load_config_file
// ---------------------------------------------------------------------------
// Loads a logger configuration from a YAML file and returns a LoggerConfig
// object.
//
// Example usage:
//     LoggerConfig cfg = load_config_file("logger.yaml");
//
// All validation and parsing logic (sinks, modules, levels, etc.) happens
// inside the corresponding .cpp file.
// ---------------------------------------------------------------------------
LoggerConfig load_config_file(const std::string& path);

}  // namespace rover_logger

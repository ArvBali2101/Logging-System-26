#include "rover_logger/config.hpp"

#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <cctype>
#include <exception>
#include <sstream>
#include <stdexcept>

namespace rover_logger {

// -----------------------------------------------------------------------------
// to_lower
// -----------------------------------------------------------------------------
// Convert a string to lowercase. Used to make log-level parsing case-insensitive
// so the user can write INFO, info, Warn, warning, etc.
// -----------------------------------------------------------------------------
static std::string to_lower(std::string_view s) {
  std::string out;
  out.reserve(s.size());
  for (unsigned char c : s) out.push_back(static_cast<char>(std::tolower(c)));
  return out;
}

// -----------------------------------------------------------------------------
// parse_level
// -----------------------------------------------------------------------------
// Convert a human-readable level ("info", "warn", "debug") into a LogLevel enum.
// Throws std::invalid_argument for unknown strings.
// -----------------------------------------------------------------------------
LogLevel parse_level(std::string_view s) {
  const std::string k = to_lower(s);
  if (k == "trace") return LogLevel::TRACE;
  if (k == "debug") return LogLevel::DEBUG;
  if (k == "info")  return LogLevel::INFO;
  if (k == "warn" || k == "warning") return LogLevel::WARN;
  if (k == "error") return LogLevel::ERROR;
  if (k == "fatal") return LogLevel::FATAL;

  std::ostringstream oss;
  oss << "Unknown log level: \"" << s << "\"";
  throw std::invalid_argument(oss.str());
}

// -----------------------------------------------------------------------------
// Optional getters
// -----------------------------------------------------------------------------
// Helpers that check if a YAML key exists.
// If it does: return its value.
// If not: return std::nullopt.
// This keeps parsing clean and avoids repetitive “if (n[key]) …” checks.
// -----------------------------------------------------------------------------
static std::optional<bool> get_opt_bool(const YAML::Node& n, const char* key) {
  if (n[key]) return n[key].as<bool>();
  return std::nullopt;
}

static std::optional<int> get_opt_int(const YAML::Node& n, const char* key) {
  if (n[key]) return n[key].as<int>();
  return std::nullopt;
}

static std::optional<std::size_t> get_opt_size(const YAML::Node& n,
                                               const char* key) {
  if (n[key]) return n[key].as<std::size_t>();
  return std::nullopt;
}

static std::optional<std::string> get_opt_str(const YAML::Node& n,
                                              const char* key) {
  if (n[key]) return n[key].as<std::string>();
  return std::nullopt;
}

// -----------------------------------------------------------------------------
// parse_sink
// -----------------------------------------------------------------------------
// Parse a single sink definition from YAML into a SinkConfig.
//
// YAML example:
//
// sinks:
//   - type: terminal
//     colorize: true
//
//   - type: file
//     path: "rover_log"
//     rotation_bytes: 524288000
//     compress: true
//
// Only "type" is required. Everything else is optional and depends on sink type.
// -----------------------------------------------------------------------------
static SinkConfig parse_sink(const YAML::Node& n) {
  if (!n || !n.IsMap())
    throw std::runtime_error("sink entry must be a map");

  SinkConfig sc{};

  // Required key: type
  if (!n["type"])
    throw std::runtime_error("sink missing required key: type");
  sc.type = n["type"].as<std::string>();

  // Optional keys (varies by sink type)
  sc.colorize       = get_opt_bool(n, "colorize");
  sc.path           = get_opt_str(n,  "path");
  sc.rotation_bytes = get_opt_size(n, "rotation_bytes");
  sc.rotate_keep    = get_opt_int(n,  "rotate_keep");
  sc.compress       = get_opt_bool(n, "compress");
  sc.host           = get_opt_str(n,  "host");
  sc.port           = get_opt_int(n,  "port");

  return sc;
}

// -----------------------------------------------------------------------------
// load_config_file
// -----------------------------------------------------------------------------
// Load the full logger configuration from a YAML file.
//
// Performs validation:
//  - file must load
//  - top-level YAML must be a map
//  - "sinks" must be a sequence
//  - "modules" must be a mapping of name → level
//
// Returns a fully-populated LoggerConfig object.
// -----------------------------------------------------------------------------
LoggerConfig load_config_file(const std::string& path) {
  YAML::Node root;

  // Load YAML from disk
  try {
    root = YAML::LoadFile(path);
  } catch (const std::exception& e) {
    std::ostringstream oss;
    oss << "Failed to load YAML \"" << path << "\": " << e.what();
    throw std::runtime_error(oss.str());
  }

  // Validate root
  if (!root || !root.IsMap()) {
    throw std::runtime_error("Top-level YAML must be a mapping/object");
  }

  LoggerConfig cfg{};

  // Global logging level 
  if (root["level"]) {
    cfg.level = parse_level(root["level"].as<std::string>());
  }

  // Queue size 
  if (root["max_queue"]) {
    const auto val = root["max_queue"].as<long long>();
    if (val <= 0)
      throw std::runtime_error("max_queue must be positive");
    cfg.max_queue = static_cast<std::size_t>(val);
  }

  // Parse sinks array
  if (root["sinks"]) {
    const YAML::Node& arr = root["sinks"];
    if (!arr.IsSequence())
      throw std::runtime_error("sinks must be a sequence");

    cfg.sinks.reserve(arr.size());
    for (const auto& sn : arr) {
      cfg.sinks.push_back(parse_sink(sn));
    }
  }

  // Parse per-module log levels
  if (root["modules"]) {
    const YAML::Node& mods = root["modules"];
    if (!mods.IsMap())
      throw std::runtime_error("modules must be a map of name->level");

    for (const auto& kv : mods) {
      const std::string name = kv.first.as<std::string>();
      const std::string lvl  = kv.second.as<std::string>();
      cfg.modules.emplace(name, parse_level(lvl));
    }
  }

  return cfg;
}

} // namespace rover_logger

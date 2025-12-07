#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "rover_logger/config.hpp"
#include "rover_logger/logger.hpp"
#include "rover_logger/terminal_sink.hpp"

namespace rover_logger {

// Build a single sink described by SinkConfig.
std::shared_ptr<ILogSink> make_sink(const SinkConfig& cfg);

// Build all sinks described in LoggerConfig.
std::vector<std::shared_ptr<ILogSink>> make_all_sinks(const LoggerConfig& cfg);

}  // namespace rover_logger

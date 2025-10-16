#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "rover_logger/config.hpp"
#include "rover_logger/logger.hpp"      
#include "rover_logger/terminal_sink.hpp"

namespace rover_logger {
std::shared_ptr<ILogSink> make_sink(const SinkConfig& cfg);
std::vector<std::shared_ptr<ILogSink>> make_all_sinks(const LoggerConfig& cfg);

}
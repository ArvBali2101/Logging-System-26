#pragma once
#include <string>
#include <string_view>

#include "rover_logger/log_level.hpp"
#include "rover_logger/log_message.hpp"

namespace rover_logger {

std::string to_json_line(const LogMessage& msg);
std::string json_escape(std::string_view s);
std::string iso8601_utc_ms(const LogMessage::clock::time_point& tp);

}  // namespace rover_logger

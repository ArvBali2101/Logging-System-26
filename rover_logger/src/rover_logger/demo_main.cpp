#include <iostream>
#include <memory>

#include "rover_logger/logger.hpp"
#include "rover_logger/log_level.hpp"
#include "rover_logger/log_message.hpp"
#include "rover_logger/terminal_sink.hpp"
#include "rover_logger/api.hpp"

using namespace rover_logger;

int main() {
    std::cout << "[DEMO] Starting rover_logger demo...\n";

    // 1. Create logger with queue size
    Logger logger(4096);

    // 2. Attach a terminal sink
    auto term = std::make_shared<TerminalSink>(true);
    logger.add_sink(term);

    // 3. Demonstrate logging using formatted printf-style API
    log_printf(logger, LogLevel::INFO,  "nav",   "Navigation online: x=%d y=%d", 12, 34);
    log_printf(logger, LogLevel::WARN,  "drive", "Low battery: %.2f volts", 6.52);
    log_printf(logger, LogLevel::ERROR, "vision","Camera failure: code %d", -1);

    // 4. Demonstrate logging by creating a LogMessage directly
    LogMessage msg(LogLevel::DEBUG, "system", "Debugging message from system module");
    logger.log(msg);

    std::cout << "[DEMO] Check above: logs were printed successfully.\n";

    return 0;
}

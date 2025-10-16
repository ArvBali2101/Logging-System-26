#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "rover_logger/config.hpp"
#include "rover_logger/log_level.hpp"
#include "rover_logger/log_message.hpp"
#include "rover_logger/logger.hpp"
#include "rover_logger/sink_factory.hpp"

using namespace std::chrono_literals;
using rover_logger::Logger;
using rover_logger::LoggerConfig;
using rover_logger::LogLevel;
using rover_logger::LogMessage;
using rover_logger::make_all_sinks;

int main() {
  try {
    // 1) Load YAML config
    const std::string cfg_path = "config/logger.yaml";
    LoggerConfig cfg = rover_logger::load_config_file(cfg_path);

    // 2) Build the logger (bounded). If loader gave 0, fall back to 2048.
    std::size_t qcap =
        (cfg.max_queue == 0) ? static_cast<std::size_t>(2048) : cfg.max_queue;
    Logger logger(qcap);

    // 3) Attach sinks from YAML (terminal + rotating file)
    auto sinks = make_all_sinks(cfg);
    for (auto& s : sinks) logger.add_sink(s);

    // 4) Set global min level from YAML (assumed filled by loader; fallback
    // INFO)
    LogLevel global_min = cfg.level;
    logger.set_min_level(global_min);

    // 5) Simulate rover subsystems (three producers)
    std::vector<std::thread> producers;
    std::vector<std::string> modules = {"/drive", "/vision", "/nav"};
    std::atomic<bool> running{true};

    auto producer_fn = [&](std::string module, int id) {
      std::mt19937 rng(42 + id);
      std::uniform_int_distribution<int> jitter(10, 60);  // ms
      std::uniform_int_distribution<int> lvl(0, 5);       // TRACE..FATAL

      for (int i = 0; i < 300; ++i) {  // ~ few seconds of activity
        LogLevel L = static_cast<LogLevel>(lvl(rng));
        std::string msg = "tick#" + std::to_string(i) + " from " + module;
        logger.log(LogMessage{L, module, msg});
        std::this_thread::sleep_for(std::chrono::milliseconds(jitter(rng)));
        if (!running.load(std::memory_order_relaxed)) break;
      }
    };

    for (int i = 0; i < static_cast<int>(modules.size()); ++i) {
      producers.emplace_back(producer_fn, modules[i], i);
    }

    // 6) Let it run a bit, then finish
    std::this_thread::sleep_for(5s);
    running.store(false, std::memory_order_relaxed);
    for (auto& t : producers) t.join();

    // 7) Print metrics
    std::cout << "[demo] processed_total=" << logger.processed_total()
              << " dropped_total=" << logger.dropped_total()
              << " queue_peak=" << logger.queue_size_peak() << "\n";

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "[demo] error: " << e.what() << "\n";
    return 1;
  }
}

#include <atomic>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "rover_logger/api.hpp"
#include "rover_logger/config.hpp"
#include "rover_logger/log_level.hpp"
#include "rover_logger/logger.hpp"
#include "rover_logger/sink_factory.hpp"

using namespace std::chrono_literals;

int main() {
  try {
    const std::string cfg_path = "config/logger.yaml";
    rover_logger::LoggerConfig cfg = rover_logger::load_config_file(cfg_path);

    std::size_t qcap =
        (cfg.max_queue == 0) ? static_cast<std::size_t>(2048) : cfg.max_queue;
    rover_logger::Logger logger(qcap);

    auto sinks = rover_logger::make_all_sinks(cfg);
    for (auto& s : sinks) logger.add_sink(s);

    logger.set_min_level(cfg.level);
    logger.apply_module_config(cfg.modules);

    std::vector<std::thread> producers;
    std::vector<std::string> modules = {"/drive", "/vision", "/nav"};
    std::atomic<bool> running{true};

    auto producer_fn = [&](std::string module, int id) {
      std::mt19937 rng(42 + id);
      std::uniform_int_distribution<int> jitter(10, 60);
      std::uniform_int_distribution<int> lvl(0, 5);

      for (int i = 0; i < 300; ++i) {
        auto L = static_cast<rover_logger::LogLevel>(lvl(rng));
        RVLOG_INFO(logger, module, "tick=%d level=%d", i, static_cast<int>(L));
        std::this_thread::sleep_for(std::chrono::milliseconds(jitter(rng)));
        if (!running.load(std::memory_order_relaxed)) break;
      }
    };

    for (int i = 0; i < static_cast<int>(modules.size()); ++i) {
      producers.emplace_back(producer_fn, modules[i], i);
    }

    std::this_thread::sleep_for(5s);
    running.store(false, std::memory_order_relaxed);
    for (auto& t : producers) t.join();

    std::cout << "[demo] processed_total=" << logger.processed_total()
              << " dropped_total=" << logger.dropped_total()
              << " queue_peak=" << logger.queue_size_peak() << "\n";

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "[demo] error: " << e.what() << "\n";
    return 1;
  }
}

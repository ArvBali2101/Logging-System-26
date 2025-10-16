#include "rover_logger/logger.hpp"

namespace rover_logger {

Logger::Logger(std::size_t max_queue) : queue_(max_queue) {
  worker_ = std::thread(&Logger::worker, this);
}

Logger::~Logger() {
  running_.store(false, std::memory_order_relaxed);
  queue_.request_stop();
  if (worker_.joinable()) worker_.join();
  for (auto& s : sinks_) s->flush();
}

void Logger::log(LogMessage msg) {
  if (static_cast<int>(msg.level) <
      static_cast<int>(min_level_.load(std::memory_order_relaxed))) {
    return;
  }
  bool dropped = queue_.push_drop_oldest(std::move(msg));
  if (dropped) dropped_total_.fetch_add(1, std::memory_order_relaxed);
}

void Logger::worker() {
  LogMessage msg{LogLevel::INFO, "_bootstrap", ""};
  while (running_.load(std::memory_order_relaxed)) {
    if (!queue_.pop_wait(msg)) break;
    for (auto& s : sinks_) {
      s->write(msg);
    }
    processed_total_.fetch_add(1, std::memory_order_relaxed);
  }
}

} 
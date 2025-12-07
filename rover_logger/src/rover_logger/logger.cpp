#include "rover_logger/logger.hpp"

namespace rover_logger {

Logger::Logger(std::size_t max_queue) : queue_(max_queue) {
  // Start the background worker thread.
  worker_ = std::thread(&Logger::worker, this);
}

Logger::~Logger() {
  // Ask worker to stop, then drain/flush.
  running_.store(false, std::memory_order_relaxed);
  queue_.request_stop();
  if (worker_.joinable()) worker_.join();
  for (auto& s : sinks_) s->flush();
}

void Logger::set_module_level(const std::string& module, LogLevel lv) {
  std::scoped_lock lk(modules_mutex_);
  module_min_levels_[module] = lv;
}

void Logger::clear_module_level(const std::string& module) {
  std::scoped_lock lk(modules_mutex_);
  module_min_levels_.erase(module);
}

void Logger::clear_all_module_levels() {
  std::scoped_lock lk(modules_mutex_);
  module_min_levels_.clear();
}

void Logger::apply_module_config(
    const std::unordered_map<std::string, LogLevel>& mods) {
  std::scoped_lock lk(modules_mutex_);
  module_min_levels_ = mods;
}

LogLevel Logger::effective_min_level(const std::string& module) const {
  std::scoped_lock lk(modules_mutex_);
  auto it = module_min_levels_.find(module);
  if (it != module_min_levels_.end()) {
    return it->second;
  }
  return min_level_.load(std::memory_order_relaxed);
}

void Logger::log(LogMessage msg) {
  // Filter by global+module level first (cheap).
  LogLevel min_lv = effective_min_level(msg.module);
  if (static_cast<int>(msg.level) < static_cast<int>(min_lv)) {
    return;
  }

  bool dropped = queue_.push_drop_oldest(std::move(msg));
  if (dropped) {
    dropped_total_.fetch_add(1, std::memory_order_relaxed);
  }
}

void Logger::worker() {
  LogMessage msg{LogLevel::INFO, "_bootstrap", ""};
  while (running_.load(std::memory_order_relaxed)) {
    if (!queue_.pop_wait(msg)) break;  // stop requested and queue empty

    for (auto& s : sinks_) {
      s->write(msg);
    }
    processed_total_.fetch_add(1, std::memory_order_relaxed);
  }
}

}  // namespace rover_logger

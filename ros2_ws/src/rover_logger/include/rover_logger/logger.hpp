#pragma once
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include "rover_logger/log_level.hpp"
#include "rover_logger/log_message.hpp"

namespace rover_logger {

class ILogSink {
 public:
  virtual ~ILogSink() = default;
  virtual void write(const LogMessage& msg) = 0;
  virtual void flush() {}
};

template <class T>
class BoundedQueue {
 public:
  explicit BoundedQueue(std::size_t cap) : cap_(cap) {}

  bool push_drop_oldest(T&& item) {
    std::scoped_lock lk(m_);
    bool dropped = false;
    if (q_.size() >= cap_) {
      q_.pop_front();
      dropped = true;
    }
    q_.emplace_back(std::move(item));
    if (q_.size() > peak_) peak_ = q_.size();
    cv_.notify_one();
    return dropped;
  }
  bool pop_wait(T& out) {
    std::unique_lock lk(m_);
    cv_.wait(lk, [&] { return stop_ || !q_.empty(); });
    if (stop_ && q_.empty()) return false;
    out = std::move(q_.front());
    q_.pop_front();
    return true;
  }

  void request_stop() {
    {
      std::scoped_lock lk(m_);
      stop_ = true;
    }
    cv_.notify_all();
  }

  std::size_t peak() const {
    std::scoped_lock lk(m_);
    return peak_;
  }

 private:
  const std::size_t cap_;
  mutable std::mutex m_;
  std::condition_variable cv_;
  std::deque<T> q_;
  bool stop_ = false;
  std::size_t peak_ = 0;
};

class Logger {
 public:
  explicit Logger(std::size_t max_queue = 4096);
  ~Logger();

  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  void add_sink(std::shared_ptr<ILogSink> sink) {
    sinks_.push_back(std::move(sink));
  }

  void set_min_level(LogLevel lv) {
    min_level_.store(lv, std::memory_order_relaxed);
  }

  void log(LogMessage msg);

  std::uint64_t dropped_total() const {
    return dropped_total_.load(std::memory_order_relaxed);
  }
  std::uint64_t processed_total() const {
    return processed_total_.load(std::memory_order_relaxed);
  }
  std::size_t queue_size_peak() const { return queue_.peak(); }

 private:
  void worker();

  std::vector<std::shared_ptr<ILogSink>> sinks_;
  BoundedQueue<LogMessage> queue_;
  std::thread worker_;
  std::atomic<bool> running_{true};

  std::atomic<LogLevel> min_level_{LogLevel::TRACE};
  std::atomic<std::uint64_t> dropped_total_{0};
  std::atomic<std::uint64_t> processed_total_{0};
};

}  
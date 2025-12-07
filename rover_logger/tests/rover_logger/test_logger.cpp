#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "rover_logger/logger.hpp"

using namespace rover_logger;

// A simple sink for testing: counts writes.
class CountingSink : public ILogSink {
 public:
  void write(const LogMessage& msg) override {
    (void)msg;  // we don't need content here
    count_.fetch_add(1, std::memory_order_relaxed);
  }
  std::uint64_t count() const { return count_.load(std::memory_order_relaxed); }

 private:
  std::atomic<std::uint64_t> count_{0};
};

int main() {
  // Test 1: basic throughput with no drops expected (big queue)
  {
    Logger log(1 << 16);  // large queue
    auto sink = std::make_shared<CountingSink>();
    log.add_sink(sink);
    log.set_min_level(LogLevel::TRACE);

    const int producers = 4;
    const int per = 5000;
    std::vector<std::thread> threads;
    for (int p = 0; p < producers; ++p) {
      threads.emplace_back([&log, p]() {
        for (int i = 0; i < per; ++i) {
          log.log(LogMessage{static_cast<LogLevel>(i % 6),
                             "mod" + std::to_string(p),
                             "msg#" + std::to_string(i)});
        }
      });
    }
    for (auto& t : threads) t.join();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    auto processed = log.processed_total();
    auto dropped = log.dropped_total();
    assert(processed > 0);
    assert(dropped == 0);  // big queue => no backpressure expected
    assert(sink->count() == processed);
  }

  // Test 2: backpressure behavior (tiny queue => expect drops)
  {
    Logger log(32);  // tiny queue to force drops
    auto sink = std::make_shared<CountingSink>();
    log.add_sink(sink);
    log.set_min_level(LogLevel::TRACE);

    const int producers = 4;
    const int per = 5000;
    std::vector<std::thread> threads;
    for (int p = 0; p < producers; ++p) {
      threads.emplace_back([&log, p]() {
        for (int i = 0; i < per; ++i) {
          log.log(LogMessage{static_cast<LogLevel>(i % 6),
                             "mod" + std::to_string(p),
                             "spam#" + std::to_string(i)});
        }
      });
    }
    for (auto& t : threads) t.join();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    auto processed = log.processed_total();
    auto dropped = log.dropped_total();

    // With a tiny queue and heavy spam, we should have dropped some
    assert(processed > 0);
    assert(dropped > 0);
    assert(sink->count() == processed);
    assert(log.queue_size_peak() <= 32);
  }

  // Test 3: level filter (min=ERROR => lower levels ignored at enqueue)
  {
    Logger log(1024);
    auto sink = std::make_shared<CountingSink>();
    log.add_sink(sink);
    log.set_min_level(LogLevel::ERROR);

    for (int i = 0; i < 1000; ++i) {
      // Only every 6th is ERROR (i%6==4) or FATAL (5)
      log.log(LogMessage{static_cast<LogLevel>(i % 6), "mod", "filter-test"});
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto processed = log.processed_total();
    // Roughly ~2/6 of 1000 -> around 333, but we avoid exactness since
    // interleaving varies.
    assert(processed > 0 && processed < 600);
    assert(sink->count() == processed);
    assert(log.dropped_total() ==
           0);  // large enough queue; no backpressure needed here
  }

  std::cout << "OK: test_logger passed.\n";
  return 0;
}

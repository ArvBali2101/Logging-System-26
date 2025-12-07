#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

#include "rover_logger/sink_factory.hpp"
#include "rover_logger/logger.hpp"
#include "rover_logger/terminal_sink.hpp"

using namespace rover_logger;

int main() {
  // 1) Terminal sink should construct and work
  {
    SinkConfig term{};
    term.type = "terminal";
    term.colorize = true;

    auto s = make_sink(term);
    assert(s && "terminal sink should be created");

    Logger L(64);
    L.add_sink(s);
    L.set_min_level(LogLevel::TRACE);
    L.log(LogMessage{LogLevel::INFO, "demo.factory", "factory-test"});
  }

  // 2) File sink (via adapter) should also construct
  {
    SinkConfig f{};
    f.type = "file";
    f.path = std::string("rover_log");
    f.rotation_bytes = 1024u; // tiny threshold

    auto s = make_sink(f);
    assert(s && "file sink should be created via adapter");
  }

  // 3) Network sink must throw (not implemented)
  {
    bool caught = false;
    try {
      SinkConfig n{};
      n.type = "network";
      (void)make_sink(n);
    } catch (const std::exception& e) {
      // Expect our "not implemented" message or any runtime error
      caught = true;
      std::cerr << "caught expected error: " << e.what() << "\n";
    }
    assert(caught && "network sink should throw (not implemented)");
  }

  // 4) Unknown sink type must also throw
  {
    bool caught = false;
    try {
      SinkConfig b{};
      b.type = "bogus";
      (void)make_sink(b);
    } catch (...) {
      caught = true;
    }
    assert(caught && "unknown sink type must throw");
  }

  std::cout << "OK: test_sink_factory passed.\n";
  return 0;
}

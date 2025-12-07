#include <cassert>
#include <filesystem>
#include <iostream>
#include "rover_logger/file_rotation_adapter.hpp"
#include "rover_logger/log_message.hpp"

namespace fs = std::filesystem;
using namespace rover_logger;

int main() {
  // clean old logs from cwd (optional)
  for (auto& e : fs::directory_iterator(".")) {
    auto n = e.path().filename().string();
    if (n.rfind("rover_log_", 0) == 0 && n.find(".log") != std::string::npos) fs::remove(e);
  }

  FileRotationAdapterOptions opt;
  opt.base_filename = "rover_log";
  opt.rotation_bytes = 400; // tiny to force rotation several times
  opt.format = AdaptFormat::JSON;

  FileRotationAdapter adapter(opt);

  for (int i = 0; i < 60; ++i)
    adapter.write(LogMessage{LogLevel::INFO, "merge.demo", "line-" + std::to_string(i) + std::string(40,'x')});

  // Expect base and at least one rotated file
  bool base = fs::exists("rover_log_0.log");
  bool rotated = false;
  for (auto& e : fs::directory_iterator(".")) {
    auto n = e.path().filename().string();
    if (n.rfind("rover_log_", 0) == 0 && n != "rover_log_0.log") rotated = true;
  }
  assert(base && rotated);

  std::cout << "OK: test_file_rotation_adapter passed.\n";
  return 0;
}

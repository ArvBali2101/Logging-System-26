#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <utility>

#include "rover_logger/json_formatter.hpp"
#include "rover_logger/log_message.hpp"
#include "rover_logger/logger.hpp"

// Teammate headers – global-namespace FileRotationSink.
#include "rover_logger/FileRotationSink.h"

namespace rover_logger {

// Choose between human-readable or JSON in the rotating files.
enum class AdaptFormat { Text, JSON };

struct FileRotationAdapterOptions {
  std::string base_filename;  // e.g. "rover_log"
  std::size_t rotation_bytes; // e.g. 500 MB
  AdaptFormat format = AdaptFormat::JSON;
};

// Wrap teammate's FileRotationSink so it can accept LogMessage.
class FileRotationAdapter final : public ILogSink {
 public:
  explicit FileRotationAdapter(FileRotationAdapterOptions opt)
      : opt_(std::move(opt)),
        sink_(std::make_unique<FileRotationSink>(
            opt_.base_filename, opt_.rotation_bytes)) {}

  void write(const LogMessage& msg) override {
    std::scoped_lock lk(m_);
    if (opt_.format == AdaptFormat::JSON) {
      sink_->write(to_json_line(msg));
    } else {
      std::string line;
      line.reserve(32 + msg.module.size() + msg.text.size());
      line.append("[")
          .append(std::string(to_string(msg.level)))
          .append("] (")
          .append(msg.module)
          .append(") ")
          .append(msg.text);
      sink_->write(line);
    }
  }

  void flush() override {
    // teammate sink flushes on std::endl
  }

 private:
  FileRotationAdapterOptions opt_;
  std::unique_ptr<FileRotationSink> sink_;
  std::mutex m_;
};

}  // namespace rover_logger

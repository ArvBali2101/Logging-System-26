#pragma once
#include <memory>
#include <mutex>
#include <string>

#include "rover_logger/json_formatter.hpp"
#include "rover_logger/log_message.hpp"
#include "rover_logger/logger.hpp"

// teammate headers
#include "rover_logger/FileRotationSink.h"  // global-namespace rotating sink

namespace rover_logger {

enum class AdaptFormat { Text, JSON };

struct FileRotationAdapterOptions {
  std::string base_filename;
  std::size_t rotation_bytes;
  AdaptFormat format = AdaptFormat::JSON;
};

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
    // The teammate sink flushes on std::endl automatically, so no-op here.
  }

private:
  FileRotationAdapterOptions opt_;
  std::unique_ptr<FileRotationSink> sink_;
  std::mutex m_;
};

} 

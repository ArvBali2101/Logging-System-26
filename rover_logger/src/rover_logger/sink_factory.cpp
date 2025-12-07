#include "rover_logger/sink_factory.hpp"

#include <memory>

#include "rover_logger/file_rotation_adapter.hpp"
#include "rover_logger/terminal_sink.hpp"

namespace rover_logger {

std::shared_ptr<ILogSink> make_sink(const SinkConfig& cfg) {
  if (cfg.type == "terminal") {
    const bool color = cfg.colorize.value_or(true);
    return std::make_shared<TerminalSink>(color);
  }

  if (cfg.type == "file") {
    FileRotationAdapterOptions opt;

    opt.base_filename = cfg.path.value_or("rover_log");

    // Default 500 MB rotation if not specified in YAML.
    const std::size_t default_rotation =
        500ull * 1024ull * 1024ull;  // 500 MB

    opt.rotation_bytes = cfg.rotation_bytes.value_or(default_rotation);
    opt.format = AdaptFormat::JSON;

    return std::make_shared<FileRotationAdapter>(opt);
  }

  if (cfg.type == "network") {
    // Placeholder – can be implemented later.
    throw std::runtime_error("Network sink not implemented in this build");
  }

  throw std::runtime_error("Unknown sink type: " + cfg.type);
}

std::vector<std::shared_ptr<ILogSink>> make_all_sinks(const LoggerConfig& cfg) {
  std::vector<std::shared_ptr<ILogSink>> out;
  out.reserve(cfg.sinks.size());
  for (const auto& s : cfg.sinks) {
    out.push_back(make_sink(s));
  }
  return out;
}

}  // namespace rover_logger

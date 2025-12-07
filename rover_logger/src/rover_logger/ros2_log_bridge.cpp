#include "rover_logger/ros2_log_bridge.hpp"

namespace rover_logger {

Ros2LogBridge::Ros2LogBridge(const LoggerConfig& cfg,
                             const std::string& config_path,
                             const rclcpp::NodeOptions& options)
    : rclcpp::Node("rover_logger_bridge", options),
      logger_((cfg.max_queue == 0) ? static_cast<std::size_t>(2048)
                                   : cfg.max_queue) {
  // Attach sinks (terminal + rotating files).
  sinks_ = make_all_sinks(cfg);
  for (auto& s : sinks_) {
    logger_.add_sink(s);
  }

  // Apply filter config: global + per-module.
  logger_.set_min_level(cfg.level);
  logger_.apply_module_config(cfg.modules);

  // Subscribe to /rover/log from all rover subsystems.
  sub_ = this->create_subscription<rover_msgs::msg::LogEntry>(
      "/rover/log",
      rclcpp::QoS(100).best_effort(),
      std::bind(&Ros2LogBridge::handle_log_msg, this, std::placeholders::_1));

  RCLCPP_INFO(this->get_logger(),
              "Rover logger bridge initialised with config '%s'",
              config_path.c_str());
}

void Ros2LogBridge::handle_log_msg(
    const rover_msgs::msg::LogEntry::SharedPtr msg) {
  // Convert string level from ROS into our enum.
  LogLevel lvl;
  try {
    lvl = parse_level(msg->level);
  } catch (const std::exception&) {
    lvl = LogLevel::INFO;
  }

  LogMessage lm{lvl, msg->module, msg->message};
  logger_.log(std::move(lm));
}

}  // namespace rover_logger

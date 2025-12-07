#pragma once
#include <memory>
#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>

#include "rover_logger/config.hpp"
#include "rover_logger/log_message.hpp"
#include "rover_logger/logger.hpp"
#include "rover_logger/sink_factory.hpp"

#include "rover_msgs/msg/log_entry.hpp"

namespace rover_logger {

// Node that subscribes to /rover/log and routes everything into Logger.
class Ros2LogBridge : public rclcpp::Node {
 public:
  Ros2LogBridge(const LoggerConfig& cfg,
                const std::string& config_path,
                const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

 private:
  void handle_log_msg(const rover_msgs::msg::LogEntry::SharedPtr msg);

  Logger logger_;
  std::vector<std::shared_ptr<ILogSink>> sinks_;

  rclcpp::Subscription<rover_msgs::msg::LogEntry>::SharedPtr sub_;
};

}  // namespace rover_logger

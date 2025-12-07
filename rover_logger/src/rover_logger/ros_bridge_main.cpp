#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>

#include "rover_logger/config.hpp"
#include "rover_logger/ros2_log_bridge.hpp"

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);

  std::string cfg_path = "config/logger.yaml";
  if (argc > 1) {
    cfg_path = argv[1];
  }

  auto cfg = rover_logger::load_config_file(cfg_path);
  auto node = std::make_shared<rover_logger::Ros2LogBridge>(cfg, cfg_path);

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}

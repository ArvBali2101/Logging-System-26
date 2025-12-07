cmake_minimum_required(VERSION 3.8)
project(rover_logger)

# Basic warnings
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Core ament / ROS2
find_package(ament_cmake REQUIRED)

# YAML for config parsing
find_package(yaml-cpp REQUIRED)

# Optional ROS2 bridge dependencies
find_package(rclcpp QUIET)
find_package(rover_msgs QUIET)

# -------------------------
# Core logger library
# -------------------------
add_library(rover_logger_core
  src/rover_logger/api.cpp
  src/rover_logger/config.cpp
  src/rover_logger/log_level.cpp
  src/rover_logger/log_message.cpp
  src/rover_logger/logger.cpp
  src/rover_logger/sink_factory.cpp
  src/rover_logger/terminal_sink.cpp
  src/rover_logger/FileRotationSink.cpp
  src/rover_logger/file_rotation_adapter.cpp
  src/rover_logger/json_formatter.cpp
)

target_include_directories(rover_logger_core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_compile_features(rover_logger_core PUBLIC cxx_std_17)

target_link_libraries(rover_logger_core
  yaml-cpp
)

# -------------------------
# Optional ROS2 log bridge
# -------------------------
if(rclcpp_FOUND AND rover_msgs_FOUND)
  message(STATUS "Building ROS2 log bridge: rclcpp + rover_msgs found")

  add_library(rover_logger_ros2_bridge
    src/rover_logger/ros2_log_bridge.cpp
  )

  ament_target_dependencies(rover_logger_ros2_bridge
    rclcpp
    rover_msgs
  )

  target_link_libraries(rover_logger_ros2_bridge
    rover_logger_core
  )

  add_executable(ros_bridge_main
    src/rover_logger/ros_bridge_main.cpp
  )

  target_link_libraries(ros_bridge_main
    rover_logger_ros2_bridge
  )

  install(TARGETS ros_bridge_main
    RUNTIME DESTINATION lib/${PROJECT_NAME}
  )

else()
  message(WARNING "Skipping ROS2 log bridge: rclcpp or rover_msgs missing")
endif()

# -------------------------
# Installation (core library + headers + config)
# -------------------------
install(TARGETS
  rover_logger_core
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

install(DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config
)

# Export for other packages
ament_export_include_directories(include)
ament_export_libraries(rover_logger_core)
add_executable(demo_main
  src/rover_logger/demo_main.cpp
)

target_link_libraries(demo_main
  rover_logger_core
)

ament_package()


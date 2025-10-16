CXX      ?= g++
CXXFLAGS ?= -std=gnu++20 -O2 -Wall -Wextra -Wpedantic -Wconversion -pthread
INCLUDES := -Iinclude -I.
BUILD    := build

TARGETS := \
  $(BUILD)/test_log_level \
  $(BUILD)/test_log_message \
  $(BUILD)/test_logger \
  $(BUILD)/test_terminal_sink \
  $(BUILD)/test_json_formatter \
  $(BUILD)/test_config \
  $(BUILD)/test_sink_factory \
  $(BUILD)/test_file_rotation_adapter

.PHONY: all build run clean \
        test_log_level test_log_message test_logger test_terminal_sink \
        test_json_formatter test_config test_sink_factory test_file_rotation_adapter \
        demo run_demo

all: $(TARGETS)
build: all

# ---------- Chunk 1: LogLevel ----------
$(BUILD)/test_log_level: src/log_level.cpp tests/test_log_level.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@
test_log_level: $(BUILD)/test_log_level
	./$<

# ---------- Chunk 2: LogMessage ----------
$(BUILD)/test_log_message: src/log_message.cpp tests/test_log_message.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@
test_log_message: $(BUILD)/test_log_message
	./$<

# ---------- Chunk 3: Logger (async queue + worker) ----------
$(BUILD)/test_logger: src/logger.cpp tests/test_logger.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@
test_logger: $(BUILD)/test_logger
	./$<

# ---------- Chunk 4: TerminalSink (needs Logger) ----------
$(BUILD)/test_terminal_sink: src/terminal_sink.cpp src/logger.cpp tests/test_terminal_sink.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@
test_terminal_sink: $(BUILD)/test_terminal_sink
	./$<

# ---------- Chunk 5: JSON Formatter ----------
$(BUILD)/test_json_formatter: src/json_formatter.cpp tests/test_json_formatter.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@
test_json_formatter: $(BUILD)/test_json_formatter
	./$<

# ---------- Chunk 6: YAML Config Loader (links yaml-cpp) ----------
$(BUILD)/test_config: src/config.cpp tests/test_config.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -lyaml-cpp -o $@
test_config: $(BUILD)/test_config
	./$<

# ---------- Chunk 7: Sink Factory (builds 'file' via adapter) ----------
$(BUILD)/test_sink_factory: \
    src/sink_factory.cpp \
    src/terminal_sink.cpp \
    src/logger.cpp \
    src/file_rotation_adapter.cpp \
    src/json_formatter.cpp \
    src/FileRotationSink.cpp \
    tests/test_sink_factory.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@
test_sink_factory: $(BUILD)/test_sink_factory
	./$<

# --------teammate rotating file sink ----------
$(BUILD)/test_file_rotation_adapter: \
    src/file_rotation_adapter.cpp \
    src/json_formatter.cpp \
    src/FileRotationSink.cpp \
    tests/test_file_rotation_adapter.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@
test_file_rotation_adapter: $(BUILD)/test_file_rotation_adapter
	./$<

# ---------- Demo app (realistic rover-like logging) ----------
$(BUILD)/demo_main: \
    src/demo_main.cpp \
    src/logger.cpp \
    src/terminal_sink.cpp \
    src/json_formatter.cpp \
    src/sink_factory.cpp \
    src/config.cpp \
    src/file_rotation_adapter.cpp \
    src/FileRotationSink.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -lyaml-cpp -o $@

demo: $(BUILD)/demo_main

run_demo: demo
	@echo ">>> Running rover_logger demo with config/logger.yaml"
	./$(BUILD)/demo_main

# ---------- Run whole test suite ----------
run: all
	@echo "Running test_log_level...";             ./$(BUILD)/test_log_level
	@echo "Running test_log_message...";           ./$(BUILD)/test_log_message
	@echo "Running test_logger...";                ./$(BUILD)/test_logger
	@echo "Running test_terminal_sink...";         ./$(BUILD)/test_terminal_sink
	@echo "Running test_json_formatter...";        ./$(BUILD)/test_json_formatter
	@echo "Running test_config...";                ./$(BUILD)/test_config
	@echo "Running test_sink_factory...";          ./$(BUILD)/test_sink_factory
	@echo "Running test_file_rotation_adapter..."; ./$(BUILD)/test_file_rotation_adapter

# ---------- Clean ----------
clean:
	@rm -rf $(BUILD)

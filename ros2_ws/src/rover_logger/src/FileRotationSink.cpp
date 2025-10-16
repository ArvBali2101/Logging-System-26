#include "rover_logger/FileRotationSink.h"


#include <filesystem>
#include <iostream>

FileRotationSink::FileRotationSink(const std::string& base, size_t maxSize)
    : baseFilename(base), maxFileSize(maxSize), fileIndex(0) {
  currentFile.open(baseFilename + "_0.log", std::ios::out | std::ios::trunc);
}

FileRotationSink::~FileRotationSink() {
  if (currentFile.is_open()) {
    currentFile.close();
  }
}

void FileRotationSink::rotate() {
  if (currentFile.is_open()) {
    currentFile.close();
  }
  fileIndex++;
  std::string newFilename =
      baseFilename + "_" + std::to_string(fileIndex) + ".log";
  currentFile.open(newFilename, std::ios::out | std::ios::trunc);
}

void FileRotationSink::write(const std::string& message) {
  if (!currentFile.is_open()) return;

  currentFile << message << std::endl;

  if (currentFile.tellp() >= static_cast<std::streampos>(maxFileSize)) {
    rotate();
  }
}
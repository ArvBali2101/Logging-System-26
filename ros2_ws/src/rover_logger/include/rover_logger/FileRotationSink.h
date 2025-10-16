#ifndef FILEROTATIONSINK_H
#define FILEROTATIONSINK_H

#include <fstream>
#include <string>

#include "ILogSink.h"

class FileRotationSink : public ILogSink {
 private:
  std::ofstream currentFile;
  std::string baseFilename;
  size_t maxFileSize;
  int fileIndex;

  void rotate();

 public:
  FileRotationSink(const std::string& base, size_t maxSize);
  ~FileRotationSink();
  void write(const std::string& message) override;
};

#endif  // FILEROTATIONSINK_H
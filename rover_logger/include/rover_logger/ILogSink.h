#ifndef ILOGSINK_H
#define ILOGSINK_H

#include <string>

// Interface for log sinks (file sink, console sink, etc.)
class ILogSink {
 public:
  virtual ~ILogSink() = default;
  virtual void write(const std::string& message) = 0;
};

#endif  // ILOGSINK_H

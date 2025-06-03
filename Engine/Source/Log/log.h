#pragma once

namespace RE {
enum class LogLevel { Trace, Debug, Info, Warning, Error, Critical };

class Logger {
 public:
  virtual ~Logger() = default;

  virtual void log(LogLevel level, const std::string& message) = 0;

  virtual void trace(const std::string& message) = 0;
  virtual void debug(const std::string& message) = 0;
  virtual void info(const std::string& message) = 0;
  virtual void warn(const std::string& message) = 0;
  virtual void error(const std::string& message) = 0;
  virtual void critical(const std::string& message) = 0;
};

}  // namespace RE
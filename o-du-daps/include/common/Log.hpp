#pragma once
#include <mutex>
#include <cstdio>
#include <cstdarg>

namespace odu {

enum class LogLevel { DEBUG, INFO, WARN, ERROR };

class Log {
public:
  static void set_level(LogLevel lvl);
  static void info(const char* fmt, ...);
  static void warn(const char* fmt, ...);
  static void error(const char* fmt, ...);
  static void debug(const char* fmt, ...);

private:
  static void vlog(LogLevel lvl, const char* fmt, va_list ap);
  static const char* lvl_str(LogLevel lvl);
  static std::mutex& mu();
  static LogLevel& level();
};

} // namespace odu

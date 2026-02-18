#include "common/Log.hpp"

namespace odu {

std::mutex& Log::mu() { static std::mutex m; return m; }
LogLevel& Log::level() { static LogLevel l = LogLevel::INFO; return l; }

void Log::set_level(LogLevel lvl) { level() = lvl; }

const char* Log::lvl_str(LogLevel lvl) {
  switch (lvl) {
    case LogLevel::DEBUG: return "DEBUG";
    case LogLevel::INFO:  return "INFO";
    case LogLevel::WARN:  return "WARN";
    case LogLevel::ERROR: return "ERROR";
  }
  return "UNK";
}

void Log::vlog(LogLevel lvl, const char* fmt, va_list ap) {
  if (static_cast<int>(lvl) < static_cast<int>(level())) return;
  std::lock_guard<std::mutex> lock(mu());
  std::fprintf(stderr, "[%s] ", lvl_str(lvl));
  std::vfprintf(stderr, fmt, ap);
  std::fprintf(stderr, "\n");
}

void Log::info(const char* fmt, ...)  { va_list ap; va_start(ap, fmt); vlog(LogLevel::INFO,  fmt, ap); va_end(ap); }
void Log::warn(const char* fmt, ...)  { va_list ap; va_start(ap, fmt); vlog(LogLevel::WARN,  fmt, ap); va_end(ap); }
void Log::error(const char* fmt, ...) { va_list ap; va_start(ap, fmt); vlog(LogLevel::ERROR, fmt, ap); va_end(ap); }
void Log::debug(const char* fmt, ...) { va_list ap; va_start(ap, fmt); vlog(LogLevel::DEBUG, fmt, ap); va_end(ap); }

} // namespace odu

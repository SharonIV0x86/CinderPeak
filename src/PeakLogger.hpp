#pragma once
#include <chrono>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <memory>
#include <sstream>
#include <string>

#define COLOR_RESET "\033[0m"
#define COLOR_WHITE "\033[37m"
#define COLOR_BOLD_WHITE "\033[1;37m"
#define COLOR_TRACE "\033[90m"
#define COLOR_DEBUG "\033[36m"
#define COLOR_INFO "\033[32m"
#define COLOR_WARNING "\033[33m"
#define COLOR_ERROR "\033[31m"
#define COLOR_CRITICAL "\033[1;31m"
#define COLOR_BOLD_DEBUG "\033[1;36m"
#define COLOR_BOLD_INFO "\033[1;32m"
#define COLOR_BOLD_WARN "\033[1;33m"
#define COLOR_BOLD_ERROR "\033[1;31m"
#define COLOR_BOLD_CRIT "\033[1;91m"

enum LogLevel
{
  TRACE,
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  CRITICAL
};

class Logger
{
public:
  static void shutdown()
  {
    disableFileLogging();
  }

  static void setFileLogging(const std::string &path)
  {
    auto newFile = std::make_unique<std::ofstream>(path, std::ios::app);
    if (!newFile->is_open())
    {
      return;
    }

    std::unique_ptr<std::ofstream> oldFile;
    {
      std::lock_guard<std::mutex> lock(logMutex);
      oldFile = std::move(logFile);
      logFile = std::move(newFile);
      activeLogFilePath = path;
    }
  }

  static void disableFileLogging()
  {
    std::unique_ptr<std::ofstream> oldFile;
    {
      std::lock_guard<std::mutex> lock(logMutex);
      oldFile = std::move(logFile);
      activeLogFilePath.clear();
    }
  }

  static void log(const LogLevel &level, const std::string &msg,
                  bool consoleEnabled, bool fileEnabled,
                  const std::string &logFileP)
  {
    if (!consoleEnabled && !fileEnabled)
    {
      return;
    }

    if (consoleEnabled)
    {
      logToConsole(level, msg);
    }

    if (fileEnabled)
    {
      ensureFileOpen(logFileP);
      logToFile(level, msg);
    }
  }

private:
  inline static std::mutex logMutex;
  inline static std::unique_ptr<std::ofstream> logFile;
  inline static std::string activeLogFilePath;

  static const char *levelToString(LogLevel level)
  {
    switch (level)
    {
    case LogLevel::TRACE:
      return "TRACE";
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARNING:
      return "WARN";
    case LogLevel::ERROR:
      return "ERROR";
    case LogLevel::CRITICAL:
      return "CRITICAL";
    default:
      return "UNKNOWN";
    }
  }

  static const char *levelToColor(LogLevel level)
  {
    switch (level)
    {
    case LogLevel::TRACE:
      return COLOR_TRACE;
    case LogLevel::DEBUG:
      return COLOR_BOLD_DEBUG;
    case LogLevel::INFO:
      return COLOR_BOLD_INFO;
    case LogLevel::WARNING:
      return COLOR_BOLD_WARN;
    case LogLevel::ERROR:
      return COLOR_BOLD_ERROR;
    case LogLevel::CRITICAL:
      return COLOR_BOLD_CRIT;
    default:
      return COLOR_WHITE;
    }
  }

  // Cross-platform, thread-safe helper to eliminate C4996 'localtime' unsafe
  // warning
  static std::tm getLocalTime(const std::time_t &time_res)
  {
    std::tm timeinfo;
#if defined(_MSC_VER)
    // MSVC secure alternative
    localtime_s(&timeinfo, &time_res);
#elif defined(__unix__) || defined(__APPLE__) || defined(__linux__)
    // POSIX thread-safe alternative
    localtime_r(&time_res, &timeinfo);
#else
    // Fallback if compilation environment is ambiguous
    if (auto *fallback = std::localtime(&time_res))
    {
      timeinfo = *fallback;
    }
    else
    {
      std::memset(&timeinfo, 0, sizeof(std::tm));
    }
#endif
    return timeinfo;
  }

  static std::string getTimestamp()
  {
    auto now = std::chrono::system_clock::now();
    auto t_c = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    std::tm timeinfo = getLocalTime(t_c);

    std::ostringstream oss;
    oss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << '.' << std::setw(3)
        << std::setfill('0') << ms.count();
    return oss.str();
  }

  static void ensureFileOpen(const std::string &path)
  {
    if (path.empty())
    {
      return;
    }

    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile && logFile->is_open() && activeLogFilePath == path)
    {
      return;
    }

    auto newFile = std::make_unique<std::ofstream>(path, std::ios::app);
    if (!newFile->is_open())
    {
      return;
    }

    logFile = std::move(newFile);
    activeLogFilePath = path;
  }

  static void logToConsole(LogLevel level, const std::string &msg)
  {
    std::lock_guard<std::mutex> lock(logMutex);

    std::string timestamp = getTimestamp();
    const char *levelStr = levelToString(level);
    const char *levelColor = levelToColor(level);
    std::cerr << COLOR_BOLD_WHITE << "[" << COLOR_RESET << timestamp
              << COLOR_BOLD_WHITE << "] [" << COLOR_RESET << levelColor
              << levelStr << COLOR_RESET << COLOR_BOLD_WHITE << "]"
              << COLOR_RESET << " " << msg << std::endl;
  }

  static void logToFile(LogLevel level, const std::string &msg)
  {
    std::lock_guard<std::mutex> lock(logMutex);

    if (!logFile || !logFile->is_open())
      return;

    std::string timestamp = getTimestamp();
    const char *levelStr = levelToString(level);

    *logFile << "[" << timestamp << "] [" << levelStr << "] " << msg;
    *logFile << std::endl;
  }
};
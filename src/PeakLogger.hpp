#pragma once
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
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

enum LogLevel { TRACE, DEBUG, INFO, WARNING, ERROR, CRITICAL };
enum ErrorPolicy { Throw = 1, Ignore = 2 };
enum LoggingPolicy {
  LogConsole = 1,
  Silent = 3,
  LogFile = 4,
  ConsoleAndFile = 5
};

class Logger {
public:
  inline static bool enableConsoleLogging = false;
  inline static bool enableFileLogging = false;

  static void log(const LogLevel &level, const std::string &msg,
                  const int &loggingPolicy, const std::string &logFileP) {
    // Reset logging modes on each call
    enableConsoleLogging = false;
    enableFileLogging = false;

    if (loggingPolicy == LoggingPolicy::ConsoleAndFile) {
      enableConsoleLogging = true;
      enableFileLogging = true;
      ensureFileOpen(logFileP);
    } else if (loggingPolicy == LoggingPolicy::LogConsole) {
      enableConsoleLogging = true;
    } else if (loggingPolicy == LoggingPolicy::LogFile) {
      enableFileLogging = true;
      ensureFileOpen(logFileP);
    }

    if (enableConsoleLogging)
      logToConsole(level, msg);

    if (enableFileLogging)
      logToFile(level, msg, __FILE__, __LINE__);
  }

  static void shutdown() {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
      logFile.close();
    }
  }

private:
  inline static std::mutex logMutex;
  inline static std::ofstream logFile;

  static const char *levelToString(LogLevel level) {
    switch (level) {
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

  static const char *levelToColor(LogLevel level) {
    switch (level) {
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

  static std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto t_c = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t_c), "%Y-%m-%d %H:%M:%S") << '.'
        << std::setw(3) << std::setfill('0') << ms.count();
    return oss.str();
  }

  static void ensureFileOpen(const std::string &path) {
    if (!logFile.is_open()) {
      logFile.open(path, std::ios::app);
    }
  }

  static void logToConsole(LogLevel level, const std::string &msg) {
    std::lock_guard<std::mutex> lock(logMutex);

    std::string timestamp = getTimestamp();
    const char *levelStr = levelToString(level);
    const char *levelColor = levelToColor(level);

    std::cerr << COLOR_BOLD_WHITE << "[" << COLOR_RESET << timestamp
              << COLOR_BOLD_WHITE << "] [" << COLOR_RESET << levelColor
              << levelStr << COLOR_RESET << COLOR_BOLD_WHITE << "]"
              << COLOR_RESET << " " << msg << std::endl;
  }

  static void logToFile(LogLevel level, const std::string &msg,
                        const std::string &file, int line) {
    if (!logFile.is_open())
      return;

    std::lock_guard<std::mutex> lock(logMutex);

    std::string timestamp = getTimestamp();
    const char *levelStr = levelToString(level);

    logFile << "[" << timestamp << "] [" << levelStr << "] " << msg;
    if (!file.empty() && line != -1 &&
        (level == LogLevel::CRITICAL || level == LogLevel::ERROR)) {
      logFile << " (" << file << ":" << line << ")";
    }
    logFile << std::endl;
  }
};

// for now, not removed but disabled the behavior, so that builds dont fail.
#define LOG_TRACE(msg)
//  Logger::log(LogLevel::TRACE, msg)
#define LOG_DEBUG(msg)
//  Logger::log(LogLevel::DEBUG, msg)
#define LOG_INFO(msg)
// Logger::log(LogLevel::INFO, msg)
#define LOG_WARNING(msg)
//  Logger::log(LogLevel::WARNING, msg, __FILE__, __LINE__)
#define LOG_ERROR(msg)
//  Logger::log(LogLevel::ERROR, msg, __FILE__, __LINE__)
#define LOG_CRITICAL(msg)

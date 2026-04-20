#pragma once
#include "logging/PeakLogger.hpp"
#include <atomic>
#include <mutex>
#include <string>

namespace CinderPeak {

class GraphRuntime {
private:
  std::atomic<bool> logToConsole;
  std::atomic<bool> throwExceptions;
  std::atomic<bool> fileLoggingEnabled;

  std::string logFilePath;
  mutable std::mutex fileMutex;

public:
  GraphRuntime()
      : logToConsole(false), throwExceptions(false), fileLoggingEnabled(false),
        logFilePath("") {}

  void setConsoleLogging(bool toggle) {
    logToConsole.store(toggle, std::memory_order_relaxed);
  }

  void setThrowExceptions(bool toggle) {
    throwExceptions.store(toggle, std::memory_order_relaxed);
  }

  void setFileLogging(const std::string &path) {
    {
      std::lock_guard<std::mutex> lock(fileMutex);
      logFilePath = path;
    }
    fileLoggingEnabled.store(true, std::memory_order_relaxed);
  }

  void disableFileLogging() {
    fileLoggingEnabled.store(false, std::memory_order_relaxed);
  }

  void log(const LogLevel &level, const std::string &msg) {
    bool console = logToConsole.load(std::memory_order_relaxed);
    bool file = fileLoggingEnabled.load(std::memory_order_relaxed);

    if (!console && !file)
      return;

    std::string path;
    if (file) {
      std::lock_guard<std::mutex> lock(fileMutex);
      path = logFilePath; // copy safely
    }

    Logger::log(level, msg, console, file, path);
  }
};

} // namespace CinderPeak
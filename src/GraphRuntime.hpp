#pragma once
#include "PeakLogger.hpp"
#include <atomic>
#include <mutex>
#include <string>

namespace CinderPeak {

class GraphRuntime {
private:
  std::atomic<bool> throwExceptions;
  std::atomic<bool> fileLoggingEnabled;

  std::string logFilePath;
  mutable std::mutex fileMutex;

public:
  GraphRuntime()
      : throwExceptions(false), fileLoggingEnabled(false),
        logFilePath("") {}

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
    bool file = fileLoggingEnabled.load(std::memory_order_relaxed);

    if (!file)
      return;

    std::string path;
    {
      std::lock_guard<std::mutex> lock(fileMutex);
      path = logFilePath;
    }

    Logger::log(level, msg, false, file, path);
  }
};

} // namespace CinderPeak
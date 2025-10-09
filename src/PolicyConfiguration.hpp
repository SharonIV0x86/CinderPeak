#pragma once
#include "CinderExceptions.hpp"
#include "PeakLogger.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include <iostream>
#include <memory>
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

namespace CinderPeak {

struct PolicyConfiguration {
public:
  enum ErrorPolicy { Throw = 1, Ignore = 2 };
  enum LoggingPolicy {
    LogConsole = 1,
    Silent = 3,
    LogFile = 4,
    ConsoleAndFile = 5
  };
  PolicyConfiguration(
      const ErrorPolicy &errorPolicy = ErrorPolicy::Ignore,
      const LoggingPolicy &loggingPolicy = LoggingPolicy::Silent,
      const std::string logfilePath = "") {
    this->errorPolicy = errorPolicy;
    this->loggingPolicy = loggingPolicy;
    this->logfilePath = logfilePath;
  }
  const ErrorPolicy &getErrorPolicy() const { return errorPolicy; }
  const LoggingPolicy &getLoggingPolicy() const { return loggingPolicy; }
  const std::string &getLogFilePath() const { return logfilePath; }
  //   bool shouldIncludeTimestamp() const { return includeTimestamp; }

private:
  ErrorPolicy errorPolicy = ErrorPolicy::Ignore;
  LoggingPolicy loggingPolicy = LoggingPolicy::Silent;
  std::string logfilePath = "peak_logs.log";
  //   bool includeTimestamp = false;
};

class PolicyHandler {
  std::shared_ptr<PolicyConfiguration> cfg;
  const PeakExceptions::GraphException
  handleExceptionMap(const PeakStatus &status) {
    switch (status.code()) {
    case CinderPeak::StatusCode::NOT_FOUND:
      throw PeakExceptions::NotFoundException(status.message());
      break;
    case CinderPeak::StatusCode::INVALID_ARGUMENT:
      throw PeakExceptions::InvalidArgumentException(status.message());
      break;
    case CinderPeak::StatusCode::VERTEX_ALREADY_EXISTS:
      throw PeakExceptions::VertexAlreadyExistsException(status.message());
      break;
    case CinderPeak::StatusCode::INTERNAL_ERROR:
      throw PeakExceptions::InternalErrorException(status.message());
      break;
    case CinderPeak::StatusCode::EDGE_NOT_FOUND:
      throw PeakExceptions::EdgeNotFoundException(status.message());
      break;
    case CinderPeak::StatusCode::VERTEX_NOT_FOUND:
      throw PeakExceptions::VertexNotFoundException(status.message());
      break;
    case CinderPeak::StatusCode::UNIMPLEMENTED:
      throw PeakExceptions::UnimplementedException(status.message());
      break;
    case CinderPeak::StatusCode::ALREADY_EXISTS:
      throw PeakExceptions::AlreadyExistsException(status.message());
      break;
    case CinderPeak::StatusCode::EDGE_ALREADY_EXISTS:
      throw PeakExceptions::EdgeAlreadyExistsException(status.message());
      break;
    default:
      throw PeakExceptions::UnknownException();
      break;
    }
  }

public:
  PolicyHandler(const PolicyConfiguration &cfg) {
    this->cfg = std::make_shared<PolicyConfiguration>(cfg);
  }
  inline void handleException(const PeakStatus &status) {
    if (status.isOK())
      return;
    switch (cfg->getErrorPolicy()) {
    case PolicyConfiguration::ErrorPolicy::Ignore:
      std::cout << "Set the error policy as ignore\n";
      break;
    case PolicyConfiguration::ErrorPolicy::Throw:
      std::cout << "Set the error policy as throw\n";
      throw handleExceptionMap(status);
      break;
    default:
      break;
    }
  }
  inline void log(const LogLevel &level, const std::string &message) {
    int l = static_cast<int>(cfg->getLoggingPolicy());
    Logger::log(level, message, l, cfg->getLogFilePath());
  }
};
} // namespace CinderPeak

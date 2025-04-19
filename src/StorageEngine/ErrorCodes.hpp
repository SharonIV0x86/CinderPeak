#pragma once
#include <string>

namespace CinderPeak {

enum class StatusCode {
  OK,
  NOT_FOUND,
  INVALID_ARGUMENT,
  VERTEX_ALREADY_EXISTS,
  INTERNAL_ERROR,
  EDGE_NOT_FOUND,
  VERTEX_NOT_FOUND
};

class PeakStatus {
private:
  StatusCode code_;
  std::string message_;

public:
  PeakStatus(StatusCode code, std::string message = "")
      : code_(code), message_(std::move(message)) {}

  static PeakStatus OK() { return PeakStatus(StatusCode::OK); }
  static PeakStatus NotFound(std::string msg = "Not Found") {
    return PeakStatus(StatusCode::NOT_FOUND, std::move(msg));
  }
  static PeakStatus InvalidArgument(std::string msg = "Invalid Argument") {
    return PeakStatus(StatusCode::INVALID_ARGUMENT, std::move(msg));
  }
  static PeakStatus AlreadyExists(std::string msg = "Already Exists") {
    return PeakStatus(StatusCode::VERTEX_ALREADY_EXISTS, std::move(msg));
  }
  static PeakStatus InternalError(std::string msg = "Internal Error") {
    return PeakStatus(StatusCode::INTERNAL_ERROR, std::move(msg));
  }
  static PeakStatus EdgeNotFound(std::string msg = "Edge Not Found") {
    return PeakStatus(StatusCode::EDGE_NOT_FOUND, std::move(msg));
  }
  static PeakStatus VertexNotFound(std::string msg = "Vertex Not Found") {
    return PeakStatus(StatusCode::VERTEX_NOT_FOUND, std::move(msg));
  }
  bool isOK() const { return code_ == StatusCode::OK; }
  StatusCode code() const { return code_; }
  std::string message() const { return message_; }

  std::string toString() const {
    return "[" + std::to_string(static_cast<int>(code_)) + "] " + message_;
  }
};

} // namespace CinderPeak

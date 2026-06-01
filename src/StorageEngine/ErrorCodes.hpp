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
  VERTEX_NOT_FOUND,
  UNIMPLEMENTED,
  ALREADY_EXISTS,
  EDGE_ALREADY_EXISTS,
};

enum class ErrorCode : int {
  None = 0,
  NotFound = 1,
  InvalidArgument = 2,
  VertexAlreadyExists = 3,
  InternalError = 4,
  EdgeNotFound = 5,
  VertexNotFound = 6,
  Unimplemented = 7,
  AlreadyExists = 8,
  EdgeAlreadyExists = 9,
};

inline ErrorCode toErrorCode(StatusCode sc) {
  switch (sc) {
  case StatusCode::OK:
    return ErrorCode::None;
  case StatusCode::NOT_FOUND:
    return ErrorCode::NotFound;
  case StatusCode::INVALID_ARGUMENT:
    return ErrorCode::InvalidArgument;
  case StatusCode::VERTEX_ALREADY_EXISTS:
    return ErrorCode::VertexAlreadyExists;
  case StatusCode::INTERNAL_ERROR:
    return ErrorCode::InternalError;
  case StatusCode::EDGE_NOT_FOUND:
    return ErrorCode::EdgeNotFound;
  case StatusCode::VERTEX_NOT_FOUND:
    return ErrorCode::VertexNotFound;
  case StatusCode::UNIMPLEMENTED:
    return ErrorCode::Unimplemented;
  case StatusCode::ALREADY_EXISTS:
    return ErrorCode::AlreadyExists;
  case StatusCode::EDGE_ALREADY_EXISTS:
    return ErrorCode::EdgeAlreadyExists;
  }
  return ErrorCode::InternalError;
}

inline std::string errorCodeToString(ErrorCode ec) {
  switch (ec) {
  case ErrorCode::None:
    return "OK";
  case ErrorCode::NotFound:
    return "Resource not found";
  case ErrorCode::InvalidArgument:
    return "Invalid argument";
  case ErrorCode::VertexAlreadyExists:
    return "Vertex already exists";
  case ErrorCode::InternalError:
    return "Internal error";
  case ErrorCode::EdgeNotFound:
    return "Edge not found";
  case ErrorCode::VertexNotFound:
    return "Vertex not found";
  case ErrorCode::Unimplemented:
    return "Unimplemented feature";
  case ErrorCode::AlreadyExists:
    return "Resource already exists";
  case ErrorCode::EdgeAlreadyExists:
    return "Edge already exists";
  }
  return "Unknown error";
}

class PeakStatus {
private:
  StatusCode code_;
  std::string message_;

public:
  PeakStatus(StatusCode code, std::string message = "")
      : code_(code), message_(std::move(message)) {}
  PeakStatus() = default;
  inline static PeakStatus OK() { return PeakStatus(StatusCode::OK); }
  inline static PeakStatus NotFound(std::string msg = "Not Found") {
    return PeakStatus(StatusCode::NOT_FOUND, std::move(msg));
  }
  inline static PeakStatus
  InvalidArgument(std::string msg = "Invalid Argument") {
    return PeakStatus(StatusCode::INVALID_ARGUMENT, std::move(msg));
  }
  inline static PeakStatus
  VertexAlreadyExists(std::string msg = "Vertex Already Exists") {
    return PeakStatus(StatusCode::VERTEX_ALREADY_EXISTS, std::move(msg));
  }
  inline static PeakStatus
  AlreadyExists(std::string msg = "Resource Already Exists") {
    return PeakStatus(StatusCode::ALREADY_EXISTS, std::move(msg));
  }
  inline static PeakStatus InternalError(std::string msg = "Internal Error") {
    return PeakStatus(StatusCode::INTERNAL_ERROR, std::move(msg));
  }
  inline static PeakStatus EdgeNotFound(std::string msg = "Edge Not Found") {
    return PeakStatus(StatusCode::EDGE_NOT_FOUND, std::move(msg));
  }
  inline static PeakStatus
  VertexNotFound(std::string msg = "Vertex Not Found") {
    return PeakStatus(StatusCode::VERTEX_NOT_FOUND, std::move(msg));
  }
  inline static PeakStatus Unimplemented(
      std::string msg = "Method is not implemented, there has been an error.") {
    return PeakStatus(StatusCode::UNIMPLEMENTED, std::move(msg));
  }
  inline static PeakStatus
  EdgeAlreadyExists(std::string msg = "Edge Already Exists") {
    return PeakStatus(StatusCode::EDGE_ALREADY_EXISTS, std::move(msg));
  }
  bool isOK() const { return code_ == StatusCode::OK; }
  StatusCode code() const { return code_; }
  std::string message() const { return message_; }

  std::string toString() const {
    return "[" + std::to_string(static_cast<int>(code_)) + "] " + message_;
  }
};

} // namespace CinderPeak

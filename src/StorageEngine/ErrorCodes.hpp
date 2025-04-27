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

class PeakStatus {
private:
  StatusCode code_;
  std::string message_;

public:
  PeakStatus(StatusCode code, std::string message = "")
      : code_(code), message_(std::move(message)) {}

  inline static PeakStatus OK() { return PeakStatus(StatusCode::OK); }
  inline static PeakStatus NotFound(std::string msg = "Not Found") {
    return PeakStatus(StatusCode::NOT_FOUND, std::move(msg));
  }
  inline static PeakStatus InvalidArgument(std::string msg = "Invalid Argument") {
    return PeakStatus(StatusCode::INVALID_ARGUMENT, std::move(msg));
  }
  inline static PeakStatus VertexAlreadyExists(std::string msg = "Vertex Already Exists") {
    return PeakStatus(StatusCode::VERTEX_ALREADY_EXISTS, std::move(msg));
  }
  inline static PeakStatus AlreadyExists(std::string msg = "Already Exists") {
    return PeakStatus(StatusCode::VERTEX_ALREADY_EXISTS, std::move(msg));
  }
  inline static PeakStatus InternalError(std::string msg = "Internal Error") {
    return PeakStatus(StatusCode::INTERNAL_ERROR, std::move(msg));
  }
  inline static PeakStatus EdgeNotFound(std::string msg = "Edge Not Found") {
    return PeakStatus(StatusCode::EDGE_NOT_FOUND, std::move(msg));
  }
  inline static PeakStatus VertexNotFound(std::string msg = "Vertex Not Found") {
    return PeakStatus(StatusCode::VERTEX_NOT_FOUND, std::move(msg));
  }
  inline static PeakStatus MethodNotImplemented(std::string msg = "Method is not implemented, there has been an error."){
    return PeakStatus(StatusCode::UNIMPLEMENTED, std::move(msg));
  } 
  inline static PeakStatus EdgeAlreadyExists(std::string msg = "Edge Already Exists"){
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

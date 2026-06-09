#pragma once

#include "StorageEngine/ErrorCodes.hpp"
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>

namespace CinderPeak {
namespace PeakExceptions {

/**
 * @brief Structured exception base wrapping std::runtime_error
 *        with an optional ErrorCode for programmatic handling.
 */
class GraphException : public std::runtime_error {
public:
  explicit GraphException(const std::string &message,
                          ErrorCode ec = ErrorCode::None)
      : std::runtime_error(message), error_code_(ec) {}

  ErrorCode errorCode() const noexcept { return error_code_; }

protected:
  ErrorCode error_code_;
};

class NotFoundException : public GraphException {
public:
  explicit NotFoundException(const std::string &msg)
      : GraphException("Resource not found: " + msg, ErrorCode::NotFound) {}
};

class InvalidArgumentException : public GraphException {
public:
  explicit InvalidArgumentException(const std::string &arg)
      : GraphException("Invalid argument: " + arg,
                        ErrorCode::InvalidArgument) {}
};

class VertexAlreadyExistsException : public GraphException {
public:
  explicit VertexAlreadyExistsException(const std::string &msg)
      : GraphException("Vertex already exists: " + msg,
                        ErrorCode::VertexAlreadyExists) {}
};

class EdgeAlreadyExistsException : public GraphException {
public:
  explicit EdgeAlreadyExistsException(const std::string &msg)
      : GraphException("Edge already exists: " + msg,
                        ErrorCode::EdgeAlreadyExists) {}
};

class EdgeNotFoundException : public GraphException {
public:
  explicit EdgeNotFoundException(const std::string &msg)
      : GraphException("Edge not found: " + msg, ErrorCode::EdgeNotFound) {}
};

class VertexNotFoundException : public GraphException {
public:
  explicit VertexNotFoundException(const std::string &msg)
      : GraphException("Vertex not found: " + msg,
                        ErrorCode::VertexNotFound) {}
};

class InternalErrorException : public GraphException {
public:
  explicit InternalErrorException(const std::string &msg = "")
      : GraphException("Internal error: " + msg, ErrorCode::InternalError) {}
};

class UnimplementedException : public GraphException {
public:
  explicit UnimplementedException(const std::string &msg)
      : GraphException("Unimplemented feature: " + msg,
                        ErrorCode::Unimplemented) {}
};

class AlreadyExistsException : public GraphException {
public:
  explicit AlreadyExistsException(const std::string &msg)
      : GraphException("Resource already exists: " + msg,
                        ErrorCode::AlreadyExists) {}
};

class UnknownException : public GraphException {
public:
  explicit UnknownException(
      const std::string &msg =
          "Unknown exception. Kindly report this incident.")
      : GraphException(msg, ErrorCode::InternalError) {}
};

} // namespace PeakExceptions

} // namespace CinderPeak

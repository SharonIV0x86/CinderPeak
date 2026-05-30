#pragma once

#include <cstdint>
#include <exception>
#include <string>
#include <utility>

namespace CinderPeak {

enum class ErrorCode : uint8_t {
  NONE = 0,
  DUPLICATE_VERTEX,
  INVALID_EDGE,
  GRAPH_CONFIGURATION_MISMATCH,
  VERTEX_NOT_FOUND,
  EDGE_NOT_FOUND,
  NOT_IMPLEMENTED
};

namespace PeakExceptions {

/**
 * @brief Base exception class for all CinderPeak errors.
 */
class GraphException : public std::exception {
public:
  explicit GraphException(std::string message, ErrorCode ec = ErrorCode::NONE)
      : m_message(std::move(message)), m_error_code(ec) {}

  const char *what() const noexcept override { return m_message.c_str(); }
  ErrorCode errorCode() const noexcept { return m_error_code; }

  ~GraphException() override = default;

protected:
  std::string m_message;
  ErrorCode m_error_code;
};

class NotFoundException : public GraphException {
public:
  explicit NotFoundException(const std::string &msg,
                             ErrorCode ec = ErrorCode::VERTEX_NOT_FOUND)
      : GraphException("Resource Not Found: " + msg, ec) {}
};

class InvalidArgumentException : public GraphException {
public:
  explicit InvalidArgumentException(const std::string &arg,
                                    ErrorCode ec = ErrorCode::NONE)
      : GraphException("Invalid argument: " + arg, ec) {}
};

class VertexAlreadyExistsException : public GraphException {
public:
  explicit VertexAlreadyExistsException(
      const std::string &msg, ErrorCode ec = ErrorCode::DUPLICATE_VERTEX)
      : GraphException("Vertex already exists: " + msg, ec) {}
};

class EdgeAlreadyExistsException : public GraphException {
public:
  explicit EdgeAlreadyExistsException(const std::string &msg,
                                      ErrorCode ec = ErrorCode::INVALID_EDGE)
      : GraphException("Edge already exists: " + msg, ec) {}
};

class EdgeNotFoundException : public GraphException {
public:
  explicit EdgeNotFoundException(const std::string &msg,
                                 ErrorCode ec = ErrorCode::EDGE_NOT_FOUND)
      : GraphException("Edge not found: " + msg, ec) {}
};

class VertexNotFoundException : public GraphException {
public:
  explicit VertexNotFoundException(const std::string &msg,
                                   ErrorCode ec = ErrorCode::VERTEX_NOT_FOUND)
      : GraphException("Vertex not found: " + msg, ec) {}
};

class InternalErrorException : public GraphException {
public:
  explicit InternalErrorException(const std::string &msg = "",
                                  ErrorCode ec = ErrorCode::NONE)
      : GraphException("Internal error: " + msg, ec) {}
};

class UnimplementedException : public GraphException {
public:
  explicit UnimplementedException(const std::string &msg,
                                  ErrorCode ec = ErrorCode::NOT_IMPLEMENTED)
      : GraphException("Unimplemented feature: " + msg, ec) {}
};

class AlreadyExistsException : public GraphException {
public:
  explicit AlreadyExistsException(const std::string &msg,
                                  ErrorCode ec = ErrorCode::NONE)
      : GraphException("Already Exists: " + msg, ec) {}
};

class UnknownException : public GraphException {
public:
  explicit UnknownException(
      const std::string &msg =
          "Unknown Exception. Kindly Report this incident.",
      ErrorCode ec = ErrorCode::NONE)
      : GraphException(msg, ec) {}
};
} // namespace PeakExceptions

} // namespace CinderPeak

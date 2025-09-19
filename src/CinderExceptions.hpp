#pragma once

#include <exception>
#include <string>
#include <utility>

namespace CinderPeak {
namespace PeakExceptions {

/**
 * @brief Base exception class for all CinderPeak errors.
 */
class GraphException : public std::exception {
public:
  explicit GraphException(std::string message)
      : m_message(std::move(message)) {}

  const char *what() const noexcept override { return m_message.c_str(); }

  ~GraphException() override = default;

protected:
  std::string m_message;
};

class NotFoundException : public GraphException {
public:
  explicit NotFoundException(const std::string &msg)
      : GraphException(msg + " not found.") {}
};

class InvalidArgumentException : public GraphException {
public:
  explicit InvalidArgumentException(const std::string &arg)
      : GraphException("Invalid argument: " + arg) {}
};

class VertexAlreadyExistsException : public GraphException {
public:
  explicit VertexAlreadyExistsException(const std::string &msg)
      : GraphException("Vertex already exists: " + msg) {}
};

class EdgeAlreadyExistsException : public GraphException {
public:
  explicit EdgeAlreadyExistsException(const std::string &msg)
      : GraphException("Edge already exists: " + msg) {}
};

class EdgeNotFoundException : public GraphException {
public:
  explicit EdgeNotFoundException(const std::string &msg)
      : GraphException("Edge not found: " + msg) {}
};

class VertexNotFoundException : public GraphException {
public:
  explicit VertexNotFoundException(const std::string &msg)
      : GraphException("Vertex not found: " + msg) {}
};

class InternalErrorException : public GraphException {
public:
  explicit InternalErrorException(const std::string &msg = "")
      : GraphException("Internal error" + (msg.empty() ? "" : ": " + msg)) {}
};

class UnimplementedException : public GraphException {
public:
  explicit UnimplementedException(const std::string &msg)
      : GraphException("Unimplemented feature: " + msg) {}
};

class AlreadyExistsException : public GraphException {
public:
  explicit AlreadyExistsException(const std::string &msg)
      : GraphException(msg + " already exists.") {}
};
class UnknownException : public GraphException {
public:
  explicit UnknownException(
      const std::string &msg =
          "Unknown Exception. Kindly Report this incident.")
      : GraphException(msg) {}
};
} // namespace PeakExceptions

} // namespace CinderPeak

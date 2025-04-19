#pragma once

#include <exception>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>

namespace CinderPeak {

inline std::string toString(const std::string &value) {
    return value;
}

inline std::string toString(int value) {
    return std::to_string(value);
}

// Default case for non-fundamental types or types not streamable
template <typename T>
std::string toString(const T &value) {
    return "[Type: " + std::string(typeid(T).name()) + "]";
}

class GraphException : public std::exception {
public:
    explicit GraphException(const std::string &message) : m_message(message) {}

    const char *what() const noexcept override {
        return m_message.c_str();
    }

protected:
    std::string m_message;
};

// Vertex Not Found Exception
class VertexNotFoundException : public GraphException {
public:
    explicit VertexNotFoundException(const std::string &vertexId)
        : GraphException("Vertex not found: " + vertexId) {}
};

// Edge Not Found Exception
class EdgeNotFoundException : public GraphException {
public:
    EdgeNotFoundException(const std::string &source, const std::string &target)
        : GraphException("Edge not found: " + source + " -> " + target) {}
};

// Duplicate Vertex Exception
class DuplicateVertexException : public GraphException {
public:
    explicit DuplicateVertexException(const std::string &vertexId)
        : GraphException("Duplicate vertex: " + vertexId) {}
};

// Invalid Edge Operation Exception
class InvalidEdgeOperationException : public GraphException {
public:
    InvalidEdgeOperationException(const std::string &source, const std::string &target, const std::string &reason)
        : GraphException("Invalid edge operation: " + source + " -> " + target + " | Reason: " + reason) {}
};

// Graph Operation Exception
class GraphOperationException : public GraphException {
public:
    GraphOperationException(const std::string &operation, const std::string &reason)
        : GraphException("Operation failed: " + operation + " | Reason: " + reason) {}
};

} // namespace CinderPeak

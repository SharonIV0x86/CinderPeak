#pragma once

#include <exception>
#include <string>
#include <sstream>
#include <typeinfo>
#include <utility>

namespace CinderPeak {

/**
 * @brief Utility function to convert common types to their string representation.
 *
 * Supports string and integer types explicitly. For unsupported types,
 * returns a fallback message including the type name for debugging purposes.
 */
inline std::string toString(const std::string& value) {
    return value;
}

inline std::string toString(int value) {
    return std::to_string(value);
}

template <typename T>
std::string toString(const T& /*value*/) {
    return "[Unprintable type: " + std::string(typeid(T).name()) + "]";
}

/**
 * @brief Base class for all user-facing CinderPeak exceptions.
 *
 * All graph-specific exceptions inherit from this class. It provides
 * consistent error reporting with descriptive messages.
 */
class GraphException : public std::exception {
public:
    explicit GraphException(std::string message)
        : m_message(std::move(message)) {}

    const char* what() const noexcept override {
        return m_message.c_str();
    }

    ~GraphException() override = default;

protected:
    std::string m_message;
};

// =============================================================================
//                      Specific Graph Exception Types
// =============================================================================

/**
 * @brief Exception thrown when a vertex is not found in the graph.
 *
 * Indicates that a vertex with the specified ID does not exist.
 */
class VertexNotFoundException : public GraphException {
public:
    explicit VertexNotFoundException(const std::string& vertexId)
        : GraphException("Vertex not found: ID '" + vertexId + "'. "
                         "The requested vertex does not exist in the graph. "
                         "Please verify the vertex ID is correct and that the vertex has not been removed.") {}
};

/**
 * @brief Exception thrown when an edge between two vertices is missing.
 *
 * Indicates that no edge exists between the specified source and target vertices.
 */
class EdgeNotFoundException : public GraphException {
public:
    EdgeNotFoundException(const std::string& source, const std::string& target)
        : GraphException("Edge not found: From '" + source + "' to '" + target + "'. "
                         "No edge exists between these vertices in the graph. "
                         "Verify that both vertices exist and that an edge was added between them.") {}
};

/**
 * @brief Exception thrown when attempting to add a duplicate vertex.
 *
 * Indicates that a vertex with the specified ID already exists.
 */
class DuplicateVertexException : public GraphException {
public:
    explicit DuplicateVertexException(const std::string& vertexId)
        : GraphException("Duplicate vertex detected: ID '" + vertexId + "' already exists in the graph. "
                         "Each vertex must have a unique identifier. "
                         "Consider using a different ID or updating the existing vertex.") {}
};

/**
 * @brief Exception thrown for invalid edge operations.
 *
 * Typically occurs when performing an operation involving non-existent vertices
 * or violating graph constraints such as self-loops or duplicates.
 */
class InvalidEdgeOperationException : public GraphException {
public:
    InvalidEdgeOperationException(const std::string& source,
                                  const std::string& target,
                                  const std::string& reason)
        : GraphException("Invalid edge operation: From '" + source + "' to '" + target + "'. "
                         "Reason: " + reason + ". "
                         "Ensure both vertices exist and the operation complies with graph constraints.") {}
};

/**
 * @brief General-purpose exception for graph operation failures.
 *
 * Covers miscellaneous failures not addressed by specific exception types.
 */
class GraphOperationException : public GraphException {
public:
    GraphOperationException(const std::string& operation,
                            const std::string& reason)
        : GraphException("Graph operation failed: Operation '" + operation + "'. "
                         "Reason: " + reason + ". "
                         "Check operation parameters and graph state to resolve the issue.") {}
};

} 

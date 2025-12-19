#pragma once
#include <cstddef>
#include <cstdint>

namespace CinderPeak::Algorithms {

using VertexId = uint64_t;

/**
 * Read-only graph interface for algorithms.
 * Implemented by snapshot structures (CSR, Hybrid, etc.)
 */
class GraphView {
public:
  virtual ~GraphView() = default;

  virtual size_t vertexCount() const = 0;
  virtual size_t edgeCount() const = 0;

  // CSR-style neighbor iteration
  virtual const VertexId *neighborsBegin(VertexId v) const = 0;
  virtual const VertexId *neighborsEnd(VertexId v) const = 0;
};

} // namespace CinderPeak::Algorithms

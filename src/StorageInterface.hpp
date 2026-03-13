#pragma once
#include "StorageEngine/AdjacencyList.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include "StorageEngine/HybridCSR_COO.hpp"
#include "StorageEngine/Utils.hpp"

namespace CinderPeak {
template <typename VertexType, typename EdgeType> class PeakStorageInterface {
public:
  [[nodiscard]] virtual const PeakStatus
  impl_addVertex(const VertexType &src) = 0;

  virtual const PeakStatus impl_removeVertex(const VertexType &vtx) = 0;

  // Method to remove all Vertices
  [[nodiscard]] virtual const PeakStatus impl_clearVertices() = 0;

  // Method to remove all Edges
  [[nodiscard]] virtual const PeakStatus impl_clearEdges() = 0;

  // Method to remove an Edge
  [[nodiscard]] virtual const std::pair<EdgeType, PeakStatus>
  impl_removeEdge(const VertexType &src, const VertexType &dest) = 0;

  // Method to add Edge
  [[nodiscard]] virtual const PeakStatus
  impl_addEdge(const VertexType &src, const VertexType &dest,
               const EdgeType &weight = EdgeType()) = 0;

  virtual const PeakStatus impl_updateEdge(const VertexType &src,
                                           const VertexType &dest,
                                           const EdgeType &newWeight) = 0;

  // Method to check whether a Vertex exists or not
  [[nodiscard]] virtual bool impl_hasVertex(const VertexType &v) noexcept = 0;

  [[nodiscard]] virtual bool
  impl_doesEdgeExist(const VertexType &src, const VertexType &dest,
                     const EdgeType &weight) noexcept = 0;

  [[nodiscard]] virtual bool
  impl_doesEdgeExist(const VertexType &src,
                     const VertexType &dest) noexcept = 0;

  [[nodiscard]] virtual const std::pair<EdgeType, PeakStatus>
  impl_getEdge(const VertexType &src, const VertexType &dest) = 0;

  virtual ~PeakStorageInterface() = default;
};
} // namespace CinderPeak
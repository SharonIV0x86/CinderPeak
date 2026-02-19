#pragma once
#include "StorageEngine/ErrorCodes.hpp"
#include "StorageEngine/Utils.hpp"
namespace CinderPeak {
template <typename VertexType, typename EdgeType> class PeakStorageInterface {
public:
  virtual const PeakStatus impl_addVertex(const VertexType &src) = 0;
  virtual const PeakStatus impl_removeVertex(const VertexType &vtx) = 0;

  // Method to remove all Vertices
  virtual const PeakStatus impl_clearVertices() = 0;

  // Method to remove all Edges
  virtual const PeakStatus impl_clearEdges() = 0;

  // Method to remove an Edge
  virtual const std::pair<EdgeType, PeakStatus>
  impl_removeEdge(const VertexType &src, const VertexType &dest) = 0;

  virtual const PeakStatus
  impl_addEdge(const VertexType &src, const VertexType &dest,
               const EdgeType &weight = EdgeType()) = 0;

  virtual const PeakStatus impl_updateEdge(const VertexType &src,
                                           const VertexType &dest,
                                           const EdgeType &newWeight) = 0;

  // Method to check whether a Vertex exists or not
  virtual bool impl_hasVertex(const VertexType &v) const = 0;

  virtual bool impl_doesEdgeExist(const VertexType &src, const VertexType &dest,
                                  const EdgeType &weight) const = 0;

  virtual bool impl_doesEdgeExist(const VertexType &src,
                                  const VertexType &dest) const = 0;

  virtual const std::pair<EdgeType, PeakStatus>
  impl_getEdge(const VertexType &src, const VertexType &dest) const = 0;

  virtual const std::pair<std::vector<std::pair<VertexType, EdgeType>>, PeakStatus>
  impl_getNeighbors(const VertexType &src) const = 0;

  virtual std::vector<VertexType> impl_getAllVertices() const = 0;

  virtual ~PeakStorageInterface() = default;
};
} // namespace CinderPeak

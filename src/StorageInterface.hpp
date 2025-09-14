#pragma once
#include "StorageEngine/AdjacencyList.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include "StorageEngine/HybridCSR_COO.hpp"
#include "StorageEngine/Utils.hpp"
namespace CinderPeak {
template <typename VertexType, typename EdgeType> class PeakStorageInterface {
public:
  virtual const PeakStatus impl_addVertex(const VertexType &src) = 0;
  virtual const PeakStatus impl_removeVertex(const VertexType &vtx) = 0;

  // Method to remove all Edges
  virtual const PeakStatus impl_clearEdges() = 0;

  virtual const PeakStatus
  impl_addEdge(const VertexType &src, const VertexType &dest,
               const EdgeType &weight = EdgeType()) = 0;

  virtual const PeakStatus impl_updateEdge(const VertexType &src,
                                           const VertexType &dest,
                                           const EdgeType &newWeight) = 0;

  // Method to check whether a Vertex exists or not
  virtual bool impl_hasVertex(const VertexType &v) = 0;

  virtual bool impl_doesEdgeExist(const VertexType &src, const VertexType &dest,
                                  const EdgeType &weight) = 0;

  virtual bool impl_doesEdgeExist(const VertexType &src,
                                  const VertexType &dest) = 0;

  virtual const std::pair<EdgeType, PeakStatus>
  impl_getEdge(const VertexType &src, const VertexType &dest) = 0;

  virtual ~PeakStorageInterface() = default;
};
} // namespace CinderPeak

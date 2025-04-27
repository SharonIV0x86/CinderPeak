#pragma once
#include "StorageEngine/AdjacencyList.hpp"
#include "StorageEngine/CoordinateList.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include "StorageEngine/HybridCSR_COO.hpp"
#include "StorageEngine/Utils.hpp"
namespace CinderPeak {
template <typename VertexType, typename EdgeType> class PeakStorageInterface {
public:
  // virtual void exc() const = 0;
  virtual const PeakStatus impl_addVertex(const VertexType &src) = 0;
  virtual const PeakStatus impl_addEdge(const VertexType &src,
                                        const VertexType &dest) = 0;
  virtual const PeakStatus impl_addEdge(const VertexType &src,
                                        const VertexType &dest,
                                        const EdgeType &weight) = 0;
  virtual bool impl_doesEdgeExist(const VertexType &src, const VertexType &dest,
                                  const EdgeType &weight) = 0;
  virtual const std::pair<EdgeType, PeakStatus>
  impl_getEdge(const VertexType &src, const VertexType &dest) = 0;

  virtual ~PeakStorageInterface() = default;
};
} // namespace CinderPeak

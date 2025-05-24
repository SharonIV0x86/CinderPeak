#pragma once
#include "StorageEngine/Utils.hpp"
#include "PeakLogger.hpp"
#include <memory>
#include "PeakLogger.hpp"
#include "StorageInterface.hpp"
#include "Visualizer.hpp"
namespace CinderPeak {
namespace PeakStore {

// Forward declarations
template <typename VertexType, typename EdgeType> class AdjacencyList;
template <typename VertexType, typename EdgeType> class HybridCSR_COO;
template <typename VertexType, typename EdgeType> class CoordinateList;

template <typename VertexType, typename EdgeType> class GraphContext {
public:
  std::shared_ptr<GraphInternalMetadata> metadata = nullptr;
  std::shared_ptr<GraphCreationOptions> create_options = nullptr;
  std::shared_ptr<HybridCSR_COO<VertexType, EdgeType>> hybrid_storage = nullptr;
  std::shared_ptr<AdjacencyList<VertexType, EdgeType>> adjacency_storage =
      nullptr;
  std::shared_ptr<CoordinateList<VertexType, EdgeType>> coordinate_list =
      nullptr;
  std::shared_ptr<PeakStorageInterface<VertexType, EdgeType>> active_storage = nullptr;
};

} // namespace PeakStore
} // namespace CinderPeak
#pragma once
#include "PeakLogger.hpp"
#include "StorageEngine/Utils.hpp"
#include "StorageInterface.hpp"
#include "Visualizer.hpp"
#include <memory>
namespace CinderPeak {
template <typename, typename> class PeakStorageInterface;
namespace PeakStore {

// Forward declarations
template <typename VertexType, typename EdgeType> class AdjacencyList;
template <typename VertexType, typename EdgeType> class HybridCSR_COO;

template <typename VertexType, typename EdgeType> class GraphContext {
public:
  std::shared_ptr<GraphInternalMetadata> metadata = nullptr;
  std::shared_ptr<GraphCreationOptions> create_options = nullptr;
  std::shared_ptr<HybridCSR_COO<VertexType, EdgeType>> hybrid_storage = nullptr;
  std::shared_ptr<AdjacencyList<VertexType, EdgeType>> adjacency_storage =
      nullptr;
  std::shared_ptr<PeakStorageInterface<VertexType, EdgeType>> active_storage =
      nullptr;
};

} // namespace PeakStore
} // namespace CinderPeak
#include "StorageEngine/AdjacencyList.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include "StorageEngine/HybridCSR_COO.hpp"
#include "StorageEngine/Utils.hpp"
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>
namespace CinderPeak {
namespace PeakStore {
const CinderPeak::GraphCreationOptions
    DEFAULT_GRAPH_OPTIONS({CinderPeak::GraphCreationOptions::Directed,
                           CinderPeak::GraphCreationOptions::SelfLoops});
template <typename VertexType, typename EdgeType> class PeakStore {
public:
  std::unique_ptr<HybridCSR_COO<VertexType, EdgeType>> hybrid_storage = nullptr;
  std::shared_ptr<GraphInternalMetadata> graph_metadata = nullptr;
  std::shared_ptr<GraphCreationOptions> create_options = nullptr;
  std::unique_ptr<AdjacencyList<VertexType, EdgeType>> adjacency_storage =
      nullptr;

  PeakStore(const GraphInternalMetadata &metadata,
            const GraphCreationOptions &options = DEFAULT_GRAPH_OPTIONS) {
    graph_metadata = std::make_shared<GraphInternalMetadata>(metadata);
    create_options = std::make_shared<GraphCreationOptions>(options);
    hybrid_storage = std::make_unique<HybridCSR_COO<VertexType, EdgeType>>(
        graph_metadata, create_options);
    adjacency_storage = std::make_unique<AdjacencyList<VertexType, EdgeType>>(
        graph_metadata, create_options);
  }
  void addEdge(const VertexType &src, const VertexType &dest,
               const EdgeType &weight) {
    adjacency_storage->impl_addEdge(src, dest, weight);
  }
  void addEdge(const VertexType &src, const VertexType &dest) {
    adjacency_storage->impl_addEdge(src, dest);
  }
  EdgeType getEdge(const VertexType &src, const VertexType &dest) {
    auto peakResponse = adjacency_storage->impl_getEdge(src, dest);
    if (!peakResponse.second.isOK()) {
      std::cout << peakResponse.second.message() << "\n";
      return EdgeType();
    }
    return peakResponse.first;
  }
  void addVertex(const VertexType &src) {
    adjacency_storage->impl_addVertex(src);
  }
  const std::pair<std::vector<std::pair<VertexType, EdgeType>>, PeakStatus> getNeighbors(const VertexType &src) const {
    auto peakResponse = adjacency_storage->impl_getNeighbors(src);
    if (!peakResponse.second.isOK()) {
      std::cout << peakResponse.second.message() << "\n";
    }
    return peakResponse;
  }
};

} // namespace PeakStore
} // namespace CinderPeak

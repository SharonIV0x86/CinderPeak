#pragma once
#include "StorageEngine/AdjacencyList.hpp"
#include "StorageEngine/CoordinateList.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include "StorageEngine/GraphContext.hpp"
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
  std::shared_ptr<GraphContext<VertexType, EdgeType>> ctx = nullptr;
  PeakStore(const GraphInternalMetadata &metadata,
            const GraphCreationOptions &options = DEFAULT_GRAPH_OPTIONS) {
    ctx = std::make_shared<GraphContext<VertexType, EdgeType>>();
    ctx->graph_metadata = std::make_shared<GraphInternalMetadata>(metadata);
    ctx->create_options = std::make_shared<GraphCreationOptions>(options);
    ctx->hybrid_storage =
        std::make_shared<HybridCSR_COO<VertexType, EdgeType>>(ctx);
    ctx->adjacency_storage =
        std::make_shared<AdjacencyList<VertexType, EdgeType>>(ctx);
    ctx->coordinate_list =
        std::make_shared<CoordinateList<VertexType, EdgeType>>(ctx);
    // ctx->hybrid_storage->impl_buildStructures(adjacency_storage);
  }
  PeakStatus addEdge(const VertexType &src, const VertexType &dest,
                     const EdgeType &weight) {

    PeakStatus adj_response =
        ctx->adjacency_storage->impl_addEdge(src, dest, weight);
    if (adj_response.isOK())
      ctx->graph_metadata->num_edges++;
    return adj_response;
  }
  PeakStatus addEdge(const VertexType &src, const VertexType &dest) {
    PeakStatus adj_response = ctx->adjacency_storage->impl_addEdge(src, dest);
    if (adj_response.isOK())
      ctx->graph_metadata->num_edges++;
    return adj_response;
  }
  EdgeType getEdge(const VertexType &src, const VertexType &dest) {
    auto peakResponse = ctx->adjacency_storage->impl_getEdge(src, dest);
    if (!peakResponse.second.isOK()) {
      std::cout << peakResponse.second.message() << "\n";
      return EdgeType();
    }
    return peakResponse.first;
  }
  void addVertex(const VertexType &src) {
    ctx->adjacency_storage->impl_addVertex(src);
  }
  const std::pair<std::vector<std::pair<VertexType, EdgeType>>, PeakStatus>
  getNeighbors(const VertexType &src) const {
    auto peakResponse = ctx->adjacency_storage->impl_getNeighbors(src);
    if (!peakResponse.second.isOK()) {
      std::cout << peakResponse.second.message() << "\n";
    }
    return peakResponse;
  }
};

} // namespace PeakStore
} // namespace CinderPeak

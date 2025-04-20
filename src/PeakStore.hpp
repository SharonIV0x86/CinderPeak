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
    ctx->metadata = std::make_shared<GraphInternalMetadata>(metadata);
    ctx->create_options = std::make_shared<GraphCreationOptions>(options);
    ctx->hybrid_storage =
        std::make_shared<HybridCSR_COO<VertexType, EdgeType>>();
    ctx->adjacency_storage =
        std::make_shared<AdjacencyList<VertexType, EdgeType>>();
    ctx->coordinate_list =
        std::make_shared<CoordinateList<VertexType, EdgeType>>();
    Logger::enableConsoleLogging = true;
    if (ctx->metadata->graph_type == "graph_matrix") {
      ctx->active_storage = ctx->hybrid_storage;
      LOG_DEBUG("Set active storage to Hybrid Storage.");
    } else if (ctx->metadata->graph_type == "graph_list") {
      ctx->active_storage = ctx->adjacency_storage;
      LOG_DEBUG("Set active storage to Adjacency Storage.");
    }
    LOG_INFO("Susseccfully initialized context object");
  }
  PeakStatus addEdge(const VertexType &src, const VertexType &dest,
                     const EdgeType &weight) {

    LOG_INFO("Called weighted PeakStore:addEdge");
    if (auto status = ctx->active_storage->impl_addEdge(src, dest, weight);
        !status.isOK()) {
      return status;
    }
    return PeakStatus::OK();
  }
  PeakStatus addEdge(const VertexType &src, const VertexType &dest) {
    LOG_INFO("Called unweighted PeakStore:addEdge");
    if (auto status = ctx->active_storage->impl_addEdge(src, dest);
        !status.isOK()) {
      return status;
    }
    return PeakStatus::OK();
  }
  EdgeType getEdge(const VertexType &src, const VertexType &dest) {
    LOG_INFO("Called adjacency:getEdge()");
    auto peakResponse = ctx->adjacency_storage->impl_getEdge(src, dest);
    if (!peakResponse.second.isOK()) {
      std::cout << peakResponse.second.message() << "\n";
      return EdgeType();
    }
    return peakResponse.first;
  }
  PeakStatus addVertex(const VertexType &src) {
    LOG_INFO("Called peakStore:addVertex");
    if (PeakStatus resp = ctx->active_storage->impl_addVertex(src);
        !resp.isOK())
      return resp;
    ctx->metadata->num_vertices++;
    return PeakStatus::OK();
  }
  const std::pair<std::vector<std::pair<VertexType, EdgeType>>, PeakStatus>
  getNeighbors(const VertexType &src) const {
    LOG_INFO("Called adjacency:getNeighbors()");
    auto peakResponse = ctx->adjacency_storage->impl_getNeighbors(src);
    if (!peakResponse.second.isOK()) {
      std::cout << peakResponse.second.message() << "\n";
    }
    return peakResponse;
  }
};

} // namespace PeakStore
} // namespace CinderPeak

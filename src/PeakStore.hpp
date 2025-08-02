#pragma once
#include "CinderPeak.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include "StorageEngine/GraphContext.hpp"
#include "StorageEngine/HybridCSR_COO.hpp"
#include "StorageEngine/Utils.hpp"
// #include "Visualizer.hpp"
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>
namespace CinderPeak {
template <typename VertexType, typename EdgeType> class GraphVisualizer;
namespace PeakStore {

template <typename VertexType, typename EdgeType> class PeakStore {
private:
  std::shared_ptr<GraphContext<VertexType, EdgeType>> ctx = nullptr;
  void initializeContext(const PeakNamespace::GraphInternalMetadata &metadata,
                         const GraphCreationOptions &options) {
    ctx->metadata = std::make_shared<PeakNamespace::GraphInternalMetadata>(metadata);
    ctx->create_options = std::make_shared<GraphCreationOptions>(options);
    ctx->hybrid_storage =
        std::make_shared<HybridCSR_COO<VertexType, EdgeType>>();
    ctx->adjacency_storage =
        std::make_shared<AdjacencyList<VertexType, EdgeType>>();
    // ctx->coordinate_list =
    //     std::make_shared<CoordinateList<VertexType, EdgeType>>();

    if (ctx->metadata->graph_type == "graph_matrix") {
      ctx->active_storage = ctx->adjacency_storage;
      LOG_DEBUG("Set active storage to Adjacency Storage (matrix).");
    } else if (ctx->metadata->graph_type == "graph_list") {
      ctx->active_storage = ctx->adjacency_storage;
      LOG_DEBUG("Set active storage to Adjacency Storage (list).");
    } else {
      LOG_WARNING(
          "Unknown graph type. Defaulting active storage to adjacency list.");
      ctx->active_storage = ctx->adjacency_storage;
    }
  }

public:
  PeakStore(const PeakNamespace::GraphInternalMetadata &metadata,
            const GraphCreationOptions &options =
                CinderPeak::GraphCreationOptions::getDefaultCreateOptions())
      : ctx(std::make_shared<GraphContext<VertexType, EdgeType>>()) {
    Logger::enableConsoleLogging = true;
    initializeContext(metadata, options);
    LOG_INFO("Successfully initialized context object.");
  }

  PeakStatus addEdge(const VertexType &src, const VertexType &dest,
                     const EdgeType &weight) {
    if (ctx->active_storage->impl_doesEdgeExist(src, dest, weight)) {
      LOG_DEBUG("Edge already exists");
      if (!ctx->create_options->hasOption(GraphCreationOptions::ParallelEdges))
        return PeakStatus::EdgeAlreadyExists();
    }
    LOG_INFO("Called weighted PeakStore:addEdge");
    if (auto status = ctx->active_storage->impl_addEdge(src, dest, weight);
        !status.isOK()) {
      return status;
    }

    ctx->metadata->num_edges++;
    return PeakStatus::OK();
  }
  PeakStatus addEdge(const VertexType &src, const VertexType &dest) {
    if (ctx->active_storage->impl_doesEdgeExist(src, dest)) {
      return PeakStatus::EdgeAlreadyExists();
    }
    LOG_INFO("Called unweighted PeakStore:addEdge");
    if (auto status = ctx->active_storage->impl_addEdge(src, dest);
        !status.isOK()) {
      return status;
    }
    ctx->metadata->num_edges++;
    return PeakStatus::OK();
  }
  std::pair<EdgeType, PeakStatus> getEdge(const VertexType &src,
                                          const VertexType &dest) {
    LOG_INFO("Called adjacency:getEdge()");
    auto status = ctx->adjacency_storage->impl_getEdge(src, dest);
    if (!status.second.isOK()) {
      return {EdgeType(), status.second};
    }
    return status;
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
    auto status = ctx->adjacency_storage->impl_getNeighbors(src);
    if (!status.second.isOK()) {
      std::cout << status.second.message() << "\n";
    }
    return status;
  }
  const std::shared_ptr<GraphContext<VertexType, EdgeType>> &
  getContext() const {
    return ctx;
  }
  void visualize() {
    LOG_WARNING("Unimplemented function: visualize");
    // GraphVisualizer<VertexType, EdgeType> g(
    //     ctx->adjacency_storage->getAdjList());

    // if (isTypePrimitive<VertexType>() && isTypePrimitive<EdgeType>()) {
    //   // g.print_adj_list();
    //   ctx->adjacency_storage->print_adj_list();
    //   g.visualize_primitives_graph();
    // } else {
    //   LOG_CRITICAL("Primitive type checking failed");
    // }
  }
};

} // namespace PeakStore
} // namespace CinderPeak

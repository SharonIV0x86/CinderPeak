#pragma once
#include "CinderPeak.hpp"
#include "StorageEngine/AdjacencyList.hpp"
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

template <typename VertexType, typename EdgeType> class PeakStore {
private:
  std::shared_ptr<GraphContext<VertexType, EdgeType>> ctx = nullptr;
  void initializeContext(const GraphInternalMetadata &metadata,
                         const GraphCreationOptions &options) {
    ctx->metadata = std::make_shared<GraphInternalMetadata>(metadata);
    ctx->create_options = std::make_shared<GraphCreationOptions>(options);
    ctx->hybrid_storage =
        std::make_shared<HybridCSR_COO<VertexType, EdgeType>>();
    ctx->adjacency_storage =
        std::make_shared<AdjacencyList<VertexType, EdgeType>>();
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
  PeakStore(const GraphInternalMetadata &metadata,
            const GraphCreationOptions &options =
                CinderPeak::GraphCreationOptions::getDefaultCreateOptions())
      : ctx(std::make_shared<GraphContext<VertexType, EdgeType>>()) {
    initializeContext(metadata, options);
    LOG_INFO("Successfully initialized context object.");
  }

  PeakStatus addEdge(const VertexType &src, const VertexType &dest,
                     const EdgeType &weight = EdgeType()) {
    bool isWeighted = ctx->metadata->isGraphWeighted();
    bool edgeExists;
    PeakStatus status = PeakStatus::OK();

    if (isWeighted) {
      edgeExists = ctx->active_storage->impl_doesEdgeExist(src, dest, weight);
    } else {
      edgeExists = ctx->active_storage->impl_doesEdgeExist(src, dest);
    }
    if (edgeExists) {
      if ((isWeighted && !ctx->create_options->hasOption(
                             GraphCreationOptions::ParallelEdges)) ||
          !isWeighted) {
        LOG_DEBUG("Edge already exists");
        return PeakStatus::EdgeAlreadyExists();
      }
    }

    if (isWeighted) {
      LOG_INFO("Called weighted PeakStore::addEdge");
      status = ctx->active_storage->impl_addEdge(src, dest, weight);
    } else {
      LOG_INFO("Called unweighted PeakStore::addEdge");
      status = ctx->active_storage->impl_addEdge(src, dest);
    }

    if (!status.isOK()) {
      return status;
    }

    if (ctx->active_storage->impl_doesEdgeExist(dest, src)) {
      ctx->metadata->num_parallel_edges++;
    }
    if (src == dest) {
      ctx->metadata->num_self_loops++;
    }
    ctx->metadata->num_edges++;

    return status;
  }

  std::pair<PeakStatus, EdgeType> updateEdge(const VertexType &src,
                                             const VertexType &dest,
                                             const EdgeType &newWeight) {
    LOG_INFO("Called adjacency:updateEdge()");

    PeakStatus resp =
        ctx->active_storage->impl_updateEdge(src, dest, newWeight);
    if (!resp.isOK()) {
      // failed, but still return the attempted newWeight
      return {resp, newWeight};
    }

    if (ctx->create_options->hasOption(GraphCreationOptions::Undirected)) {
      PeakStatus resp2 =
          ctx->active_storage->impl_updateEdge(dest, src, newWeight);
      if (!resp2.isOK()) {
        return {resp2, newWeight};
      }
    }

    return {PeakStatus::OK(), newWeight};
  }

  std::pair<EdgeType, PeakStatus> getEdge(const VertexType &src,
                                          const VertexType &dest) {
    LOG_INFO("Called adjacency:getEdge()");
    auto status = ctx->active_storage->impl_getEdge(src, dest);
    if (!status.second.isOK()) {
      return {EdgeType(), status.second};
    }
    return {status.first, status.second};
  }
  PeakStatus addVertex(const VertexType &src) {
    LOG_INFO("Called peakStore:addVertex");
    if (PeakStatus resp = ctx->active_storage->impl_addVertex(src);
        !resp.isOK())
      return resp;
    ctx->metadata->num_vertices++;
    return PeakStatus::OK();
  }

  // Helper method to call impl_hasVertex from AdjacencyList
  bool hasVertex(const VertexType &v) {
    LOG_INFO("Called peakStore:hasVertex");
    return ctx->active_storage->impl_hasVertex(v);
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

  PeakStatus removeVertex(const VertexType &v) {
    auto status = ctx->active_storage->impl_removeVertex(v);
    if (status.isOK()) {
      ctx->metadata->num_vertices--;
    }
    return status;
  }

  // Helper method to call impl_clearEdges from AdjacencyList
  PeakStatus clearEdges() {
    LOG_INFO("Called peakStore:clearEdges");
    auto status = ctx->active_storage->impl_clearEdges();
    if (status.isOK()) {
      ctx->metadata->num_edges = 0;
    }
    return status;
  }

  static void setConsoleLogging(const bool toggle) {
    Logger::enableConsoleLogging = toggle;
  }

  size_t numEdges() const { return ctx->metadata->num_edges; }

  size_t numVertices() const {
    LOG_INFO("Called peakStore:numVertices");
    return ctx->metadata->num_vertices;
  }

  std::string getGraphStatistics() {
    std::stringstream ss;

    if (ctx->metadata->num_vertices > 1) {
      float directed_density =
          (float)ctx->metadata->num_edges /
          (ctx->metadata->num_vertices * (ctx->metadata->num_vertices - 1));
      if (ctx->create_options->hasOption(GraphCreationOptions::Directed))
        ctx->metadata->density = directed_density;
      if (ctx->create_options->hasOption(GraphCreationOptions::Undirected))
        ctx->metadata->density = 2 * directed_density;
    }
    ss << "=== Graph Statistics ===" << std::endl;
    ss << "Vertices: " << ctx->metadata->num_vertices << std::endl;
    ss << "Edges: " << ctx->metadata->num_edges << std::endl;
    ss << "Density: " << std::fixed << std::setprecision(2)
       << ctx->metadata->density << std::endl;
    ss << "Self-loops: " << ctx->metadata->num_self_loops << std::endl;
    ss << "Parallel edges: " << ctx->metadata->num_parallel_edges << std::endl;
    return ss.str();
  }
};

} // namespace PeakStore
} // namespace CinderPeak

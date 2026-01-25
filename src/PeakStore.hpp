#pragma once
#include "Algorithms/CinderPeakAlgorithms.hpp"
#include "CinderPeak.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include "StorageEngine/GraphContext.hpp"
#include "StorageEngine/GraphStatistics.hpp"
#include "StorageEngine/HybridCSR_COO.hpp"
#include "StorageEngine/Utils.hpp"
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>
#include <fstream>
namespace CinderPeak {
namespace PeakStore {

template <typename VertexType, typename EdgeType> class PeakStore {
private:
  std::shared_ptr<GraphContext<VertexType, EdgeType>> ctx = nullptr;
  void initializeContext(const GraphInternalMetadata &metadata,
                         const GraphCreationOptions &options,
                         const PolicyConfiguration &cfg) {
    ctx->metadata = std::make_shared<GraphInternalMetadata>(metadata);
    ctx->create_options = std::make_shared<GraphCreationOptions>(options);
    ctx->hybrid_storage =
        std::make_shared<HybridCSR_COO<VertexType, EdgeType>>();
    ctx->pHandler = std::make_shared<PolicyHandler>(cfg);
    ctx->adjacency_storage =
        std::make_shared<AdjacencyList<VertexType, EdgeType>>(*ctx->pHandler);
    ctx->active_storage = ctx->adjacency_storage;
    ctx->algorithms = std::make_shared<
        Algorithms::CinderPeakAlgorithms<VertexType, EdgeType>>(
        ctx->hybrid_storage);
  }

public:
  PeakStore(const GraphInternalMetadata &metadata,
            const GraphCreationOptions &options =
                CinderPeak::GraphCreationOptions::getDefaultCreateOptions(),
            const PolicyConfiguration &cfg = PolicyConfiguration())
      : ctx(std::make_shared<GraphContext<VertexType, EdgeType>>()) {
    initializeContext(metadata, options, cfg);
    LOG_INFO("Successfully initialized context object.");
  }
  Algorithms::BFSResult<VertexType> bfs(const VertexType &src) {
    Algorithms::BFSResult<VertexType> result;
    if (!hasVertex(src)) {
      result._status =
          PeakStatus::VertexNotFound("Vertex Not Found During the BFS");
      return result;
    }
    result = std::move(ctx->algorithms->bfs(src));
    return result;
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
      ctx->metadata->updateParallelEdgeCount(UpdateOp::Add);
    }
    if (src == dest) {
      ctx->metadata->updateSelfLoopCount(UpdateOp::Add);
    }
    ctx->metadata->updateEdgeCount(UpdateOp::Add);

    return status;
  }

  std::pair<EdgeType, PeakStatus> removeEdge(const VertexType &src,
                                             const VertexType &dest) {
    LOG_INFO("Called adjacency:removeEdge()");
    auto result = ctx->active_storage->impl_removeEdge(src, dest);
    if (result.second.isOK())
      ctx->metadata->updateEdgeCount(UpdateOp::Remove);
    return result;
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
    ctx->metadata->updateVertexCount(UpdateOp::Add);

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
      ctx->metadata->updateVertexCount(UpdateOp::Remove);
    }
    return status;
  }

  // Helper method to call impl_clearVertices from AdjacencyList
  PeakStatus clearVertices() {
    LOG_INFO("Called peakStore:clearVertices");
    auto status = ctx->active_storage->impl_clearVertices();
    if (status.isOK()) {
      ctx->metadata->updateVertexCount(UpdateOp::Clear);
      ctx->metadata->updateEdgeCount(UpdateOp::Clear);
      ctx->metadata->updateParallelEdgeCount(UpdateOp::Clear);
      ctx->metadata->updateSelfLoopCount(UpdateOp::Clear);
    }
    return status;
  }

  // Helper method to call impl_clearEdges from AdjacencyList
  PeakStatus clearEdges() {
    LOG_INFO("Called peakStore:clearEdges");
    auto status = ctx->active_storage->impl_clearEdges();
    if (status.isOK()) {
      ctx->metadata->updateEdgeCount(UpdateOp::Clear);
      ctx->metadata->updateParallelEdgeCount(UpdateOp::Clear);
      ctx->metadata->updateSelfLoopCount(UpdateOp::Clear);
    }
    return status;
  }

  static void setConsoleLogging(const bool toggle) {
    Logger::enableConsoleLogging = toggle;
  }

  size_t numEdges() const { return ctx->metadata->numEdges(); }

  size_t numVertices() const {
    LOG_INFO("Called peakStore:numVertices");
    return ctx->metadata->numVertices();
  }

  std::string toDot() {
    return ctx->adjacency_storage->impl_toDot(*ctx->create_options);
  }

  void toDot(const std::string &filename) {
      if (filename.empty()) {
        LOG_ERROR("Empty filename provided for toDot output");
        return;
      }

      std::ofstream outFile(filename);
      if (!outFile) {
        LOG_ERROR("Could not open file for writing: " + filename);
        return;
      }

      std::string content = toDot();
      outFile << content;
      outFile.close();
      
      LOG_INFO("Successfully wrote DOT output to: " + filename);
  }

  const std::string getGraphStatistics() {
    bool directed;
    if (ctx->create_options->hasOption(GraphCreationOptions::Directed))
      directed = true;
    if (ctx->create_options->hasOption(GraphCreationOptions::Undirected))
      directed = false;
    return ctx->metadata->getGraphStatistics(directed);
  }
};

} // namespace PeakStore
} // namespace CinderPeak

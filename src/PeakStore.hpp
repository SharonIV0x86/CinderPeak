#pragma once
#include "Algorithms/CinderPeakAlgorithms.hpp"
#include "CinderPeak.hpp"
#include "GraphRuntime.hpp"
#include "PeakLogger.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include "StorageEngine/GraphContext.hpp"
#include "StorageEngine/GraphStatistics.hpp"
#include "StorageEngine/HybridCSR_COO.hpp"
#include "StorageEngine/Utils.hpp"
#include <fstream>
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
    ctx->runtime = std::make_shared<CinderPeak::GraphRuntime>();
    ctx->runtime->log(LogLevel::CRITICAL, "Lemon from ctx\n");
  }

public:
  PeakStore(const GraphInternalMetadata &metadata,
            const GraphCreationOptions &options =
                CinderPeak::GraphCreationOptions::getDefaultCreateOptions(),
            const PolicyConfiguration &cfg = PolicyConfiguration())
      : ctx(std::make_shared<GraphContext<VertexType, EdgeType>>()) {
    initializeContext(metadata, options, cfg);
    ctx->log(LogLevel::INFO, "Successfully initialized context object.");
    ctx->runtime->log(LogLevel::CRITICAL, "Lemon from ctx1\n");
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
    ctx->runtime->log(LogLevel::CRITICAL, "Lemon from ctx2\n");

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
        ctx->log(LogLevel::DEBUG, "Edge already exists");
        return PeakStatus::EdgeAlreadyExists();
      }
    }

    if (isWeighted) {
      ctx->log(LogLevel::INFO, "Called weighted PeakStore::addEdge");
      status = ctx->active_storage->impl_addEdge(src, dest, weight);
    } else {
      ctx->log(LogLevel::INFO, "Called unweighted PeakStore::addEdge");
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
    ctx->log(LogLevel::INFO, "Called adjacency:removeEdge()");
    auto result = ctx->active_storage->impl_removeEdge(src, dest);
    if (result.second.isOK())
      ctx->metadata->updateEdgeCount(UpdateOp::Remove);
    return result;
  }

  std::pair<PeakStatus, EdgeType> updateEdge(const VertexType &src,
                                             const VertexType &dest,
                                             const EdgeType &newWeight) {
    ctx->log(LogLevel::INFO, "Called adjacency:updateEdge()");

    PeakStatus resp =
        ctx->active_storage->impl_updateEdge(src, dest, newWeight);
    if (!resp.isOK()) {
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
    ctx->log(LogLevel::INFO, "Called adjacency:getEdge()");
    auto status = ctx->active_storage->impl_getEdge(src, dest);
    if (!status.second.isOK()) {
      return {EdgeType(), status.second};
    }
    return {status.first, status.second};
  }

  PeakStatus addVertex(const VertexType &src) {
    ctx->log(LogLevel::INFO, "Called peakStore:addVertex");
    if (PeakStatus resp = ctx->active_storage->impl_addVertex(src);
        !resp.isOK())
      return resp;
    ctx->metadata->updateVertexCount(UpdateOp::Add);

    return PeakStatus::OK();
  }

  bool hasVertex(const VertexType &v) {
    ctx->log(LogLevel::INFO, "Called peakStore:hasVertex");
    return ctx->active_storage->impl_hasVertex(v);
  }

  const std::pair<std::vector<std::pair<VertexType, EdgeType>>, PeakStatus>
  getNeighbors(const VertexType &src) const {
    ctx->log(LogLevel::INFO, "Called adjacency:getNeighbors()");
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

  PeakStatus clearVertices() {
    ctx->log(LogLevel::INFO, "Called peakStore:clearVertices");
    auto status = ctx->active_storage->impl_clearVertices();
    if (status.isOK()) {
      ctx->metadata->updateVertexCount(UpdateOp::Clear);
      ctx->metadata->updateEdgeCount(UpdateOp::Clear);
      ctx->metadata->updateParallelEdgeCount(UpdateOp::Clear);
      ctx->metadata->updateSelfLoopCount(UpdateOp::Clear);
    }
    return status;
  }

  PeakStatus clearEdges() {
    ctx->log(LogLevel::INFO, "Called peakStore:clearEdges");
    auto status = ctx->active_storage->impl_clearEdges();
    if (status.isOK()) {
      ctx->metadata->updateEdgeCount(UpdateOp::Clear);
      ctx->metadata->updateParallelEdgeCount(UpdateOp::Clear);
      ctx->metadata->updateSelfLoopCount(UpdateOp::Clear);
    }
    return status;
  }

  void setConsoleLogging(const bool toggle) {
    ctx->runtime->setConsoleLogging(toggle);
  }
  void setThrowExceptions(bool toggle) {
    ctx->runtime->setThrowExceptions(toggle);
  }
  void setFileLogging(const std::string &path) {
    ctx->runtime->setFileLogging(path);
  }
  void unsetFileLogging() { ctx->runtime->disableFileLogging(); }

  size_t numEdges() const { return ctx->metadata->numEdges(); }

  size_t numVertices() const {
    ctx->log(LogLevel::INFO, "Called peakStore:numVertices");
    return ctx->metadata->numVertices();
  }

  void toDot(const std::string &filename) {
    if (filename.empty()) {
      ctx->log(LogLevel::ERROR, "Empty filename provided for toDot output");
      return;
    }

    std::ofstream outFile(filename);
    if (!outFile) {
      ctx->log(LogLevel::ERROR, "Could not open file for writing: " + filename);
      return;
    }

    bool isDirected =
        ctx->create_options->hasOption(GraphCreationOptions::Directed);
    bool allowParallel =
        ctx->create_options->hasOption(GraphCreationOptions::ParallelEdges);

    std::string content =
        ctx->adjacency_storage->impl_toDot(isDirected, allowParallel);
    outFile << content;
    outFile.close();

    ctx->log(LogLevel::ERROR, "Successfully wrote DOT output to: " + filename);
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
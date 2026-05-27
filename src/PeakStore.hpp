#pragma once
#include "Algorithms/CinderPeakAlgorithms.hpp"
#include "CinderPeak.hpp"
#include "Constraints/Constraints.hpp"
#include "Events/DefaultListeners.hpp"
#include "GraphConstraints.hpp"
#include "GraphEvents.hpp"
#include "GraphRuntime.hpp"
#include "Operations/GraphOperations.hpp"
#include "PeakLogger.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include "StorageEngine/DebugUtils.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include "StorageEngine/GraphContext.hpp"
#include "StorageEngine/GraphStatistics.hpp"
#include "StorageEngine/HybridCSR_COO.hpp"
#include "StorageEngine/Utils.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
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
    ctx->runtime = std::make_shared<CinderPeak::GraphRuntime>();
    ctx->adjacency_storage =
        std::make_shared<AdjacencyList<VertexType, EdgeType>>(*ctx->runtime);
    ctx->active_storage = ctx->adjacency_storage;
    ctx->algorithms = std::make_shared<
        Algorithms::CinderPeakAlgorithms<VertexType, EdgeType>>(
        ctx->hybrid_storage);
    ctx->runtime->log(LogLevel::CRITICAL, "Log from ctx\n");
    registerMetadataListeners(*ctx);
  }

public:
  PeakStore(const GraphInternalMetadata &metadata,
            const GraphCreationOptions &options =
                CinderPeak::GraphCreationOptions::getDefaultCreateOptions())
      : ctx(std::make_shared<GraphContext<VertexType, EdgeType>>()) {
    initializeContext(metadata, options);
    ctx->log(LogLevel::INFO, "Successfully initialized context object.");
    ctx->runtime->log(LogLevel::CRITICAL, "Log from ctx 1\n");
  }

  // Set graph name
  bool setGraphName(const std::string &name) {
    ctx->log(LogLevel::INFO, "PeakStore: Setting graph name to: " + name);
    return ctx->metadata->setGraphName(name);
  }

  // Get graph name
  std::string getGraphName() { return ctx->metadata->getGraphName(); }

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

    AddEdgeOperation<VertexType, EdgeType> op{
        *ctx,
        src,
        dest,
        weight,
        ctx->metadata->isGraphWeighted(),
        ctx->create_options->hasOption(GraphCreationOptions::Directed)};
    PeakStatus validation = validateAddEdge(op);
    if (!validation.isOK())
      return validation;
    if (op.weighted) {
      ctx->log(LogLevel::INFO, "Called weighted PeakStore::addEdge for " +
                                   weightedEdgeStr(src, dest, weight));
    } else {
      ctx->log(LogLevel::INFO, "Called unweighted PeakStore::addEdge for " +
                                   edgeStr(src, dest));
    }
    PeakStatus status;
    if (op.weighted) {
      status = ctx->active_storage->impl_addEdge(src, dest, weight);
    } else {
      status = ctx->active_storage->impl_addEdge(src, dest);
    }
    if (!status.isOK())
      return status;
    ctx->events.edgeAdded.emit({src, dest, weight});
    if (!op.directed) {
      if (op.weighted) {
        ctx->active_storage->impl_addEdge(dest, src, weight);
      } else {
        ctx->active_storage->impl_addEdge(dest, src);
      }
      ctx->events.edgeAdded.emit({dest, src, weight});
    }
    return PeakStatus::OK();
  }
  std::pair<EdgeType, PeakStatus> removeEdge(const VertexType &src,
                                             const VertexType &dest) {
    ctx->log(LogLevel::INFO,
             "Called adjacency:removeEdge() for " + edgeStr(src, dest));
    auto result = ctx->active_storage->impl_removeEdge(src, dest);
    if (result.second.isOK()) {
      GraphEvents<VertexType, EdgeType>::onEdgeRemove(*ctx, src, dest);

      bool isDirected =
          ctx->create_options->hasOption(GraphCreationOptions::Directed);
      if (!isDirected && src != dest) {
        auto rev_result = ctx->active_storage->impl_removeEdge(dest, src);
        if (rev_result.second.isOK()) {
          GraphEvents<VertexType, EdgeType>::onEdgeRemove(*ctx, dest, src);
        }
      }
    }
    return result;
  }

  std::pair<PeakStatus, EdgeType> updateEdge(const VertexType &src,
                                             const VertexType &dest,
                                             const EdgeType &newWeight) {
    ctx->log(LogLevel::INFO, "Called adjacency:updateEdge() for " +
                                 weightedEdgeStr(src, dest, newWeight));

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
    ctx->log(LogLevel::INFO,
             "Called adjacency:getEdge() for " + edgeStr(src, dest));
    auto status = ctx->active_storage->impl_getEdge(src, dest);
    if (!status.second.isOK()) {
      return {EdgeType(), status.second};
    }
    return {status.first, status.second};
  }

  PeakStatus addVertex(const VertexType &src) {
    ctx->log(LogLevel::INFO,
             "Called peakStore:addVertex for " + vertexStr(src));
    if (PeakStatus resp = ctx->active_storage->impl_addVertex(src);
        !resp.isOK())
      return resp;
    ctx->metadata->updateVertexCount(UpdateOp::Add);

    return PeakStatus::OK();
  }

  bool hasVertex(const VertexType &v) {
    ctx->log(LogLevel::INFO, "Called peakStore:hasVertex for " + vertexStr(v));
    return ctx->active_storage->impl_hasVertex(v);
  }

  const std::pair<std::vector<std::pair<VertexType, EdgeType>>, PeakStatus>
  getNeighbors(const VertexType &src) const {
    ctx->log(LogLevel::INFO,
             "Called adjacency:getNeighbors() for " + vertexStr(src));
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
    }
    return status;
  }

  PeakStatus clearEdges() {
    ctx->log(LogLevel::INFO, "Called peakStore:clearEdges");
    auto status = ctx->active_storage->impl_clearEdges();
    if (status.isOK()) {
      ctx->metadata->updateEdgeCount(UpdateOp::Clear);
    }
    return status;
  }
  void setConsoleLogging(bool toggle) {
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

    std::string content = ctx->adjacency_storage->impl_toDot(isDirected);
    outFile << content;
    outFile.close();

    ctx->log(LogLevel::INFO, "Successfully wrote DOT output to: " + filename);
  }

  const std::string getGraphStatistics() {
    bool directed;
    if (ctx->create_options->hasOption(GraphCreationOptions::Directed))
      directed = true;
    if (ctx->create_options->hasOption(GraphCreationOptions::Undirected))
      directed = false;
    return ctx->metadata->getGraphStatistics(directed);
  }
  void log(const LogLevel &level, const std::string &message) const {
    ctx->runtime->log(level, message);
  }
};

} // namespace PeakStore
} // namespace CinderPeak
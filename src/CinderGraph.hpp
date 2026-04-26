#pragma once
#include "Algorithms/CinderPeakAlgorithms.hpp"
#include "Concepts.hpp"
#include "PeakStore.hpp"
#include "StorageEngine/GraphStatistics.hpp"
#include "StorageEngine/Utils.hpp"
#include <iostream>
#include <optional>
#include <tuple>
#include <utility>

namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class PeakStore;
}
template <typename VertexType, typename EdgeType> class CinderGraph;
template <typename VertexType, typename EdgeType> class CinderGraphRowProxy {
  CinderGraph<VertexType, EdgeType> &graph;
  VertexType src;

public:
  CinderGraphRowProxy(CinderGraph<VertexType, EdgeType> &g, const VertexType &s)
      : graph(g), src(s) {}

  EdgeType operator[](const VertexType &dest) const {
    auto [optWeight, found] = graph.getEdge(src, dest);
    if (!found || !optWeight.has_value()) {
      throw std::runtime_error("Edge not found: " + src + " -> " + dest);
    }
    return *optWeight;
  }

  CinderGraphRowProxy &operator=(const EdgeType &newWeight) = delete;

  CinderGraphRowProxy &operator()(const VertexType &dest,
                                  const EdgeType &weight) {
    graph.addEdge(src, dest, weight);
    return *this;
  }

  struct EdgeAssignProxy {
    CinderGraph<VertexType, EdgeType> &graph;
    VertexType src, dest;

    EdgeAssignProxy(CinderGraph<VertexType, EdgeType> &g, const VertexType &s,
                    const VertexType &d)
        : graph(g), src(s), dest(d) {}

    EdgeAssignProxy &operator=(const EdgeType &weight) {
      graph.addEdge(src, dest, weight);
      return *this;
    }

    operator EdgeType() const {
      auto [optWeight, found] = graph.getEdge(src, dest);
      return (found && optWeight) ? *optWeight : EdgeType{};
    }
  };

  EdgeAssignProxy operator[](const VertexType &dest) {
    return EdgeAssignProxy(graph, src, dest);
  }
};

template <typename VertexType, typename EdgeType> class CinderGraph {
  std::unique_ptr<CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>
      peak_store;

  using EdgeKey = std::pair<VertexType, VertexType>;
  using WeightedEdgeKey = std::tuple<VertexType, VertexType, EdgeType>;

  // Insert / result types (Boost-like)
  using VertexAddResult = std::pair<VertexType, bool>;
  using UnweightedEdgeAddResult = std::pair<EdgeKey, bool>;
  using WeightedEdgeAddResult = std::pair<WeightedEdgeKey, bool>;

  // UpdateEdgeResult: {previousWeight, updatedFlag}
  // (previousWeight is EdgeType{} when edge missing or unknown)
  using UpdateEdgeResult = std::pair<EdgeType, bool>;

  // GetEdgeResult: {optional(weight), foundFlag}
  // Note: optional already conveys presence; bool duplicates that info but kept
  // per typedef.
  using GetEdgeResult = std::pair<std::optional<EdgeType>, bool>;

  using RemoveEdgeResult = std::pair<std::optional<EdgeType>, bool>;

public:
  CinderGraph(const GraphCreationOptions &options =
                  GraphCreationOptions::getDefaultCreateOptions()) {
    PeakStore::GraphInternalMetadata metadata(
        "cinder_graph", Traits::isTypePrimitive<VertexType>(),
        Traits::isTypePrimitive<EdgeType>(),
        Traits::isGraphWeighted<EdgeType>(),
        !Traits::isGraphWeighted<EdgeType>());

    peak_store = std::make_unique<PeakStore::PeakStore<VertexType, EdgeType>>(
        metadata, options);
  }
  VertexAddResult addVertex(const VertexType &v) {
    peak_store->log(LogLevel::INFO, "API: Entering addVertex");
    auto resp = peak_store->addVertex(v);
    if (!resp.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in addVertex");
      Exceptions::handle_exception_map(resp);
      // If exceptions are disabled, handle_exception_map returns -> fallthrough
      return {v, false};
    }
    peak_store->log(LogLevel::INFO, "API: addVertex completed successfully");
    return {v, true};
  }
  bool removeVertex(const VertexType &v) {
    peak_store->log(LogLevel::INFO, "API: Entering removeVertex");
    auto resp = peak_store->removeVertex(v);
    if (!resp.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in removeVertex");
      Exceptions::handle_exception_map(resp);
      return false;
    }
    peak_store->log(LogLevel::INFO, "API: removeVertex completed successfully");
    return true;
  }
  RemoveEdgeResult removeEdge(const VertexType &src, const VertexType &dest) {
    peak_store->log(LogLevel::INFO, "API: Entering removeEdge");
    auto [data, status] = peak_store->removeEdge(src, dest);
    if (!status.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in removeEdge");
      Exceptions::handle_exception_map(status);
      return {std::nullopt, false};
    }
    peak_store->log(LogLevel::INFO, "API: removeEdge completed successfully");
    return {std::make_optional(data), true};
  }

  // Helper method to call clearVertices from PeakStore
  void clearVertices() {
    peak_store->log(LogLevel::INFO, "API: Entering clearVertices");
    auto resp = peak_store->clearVertices();
    if (!resp.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in clearVertices");
      Exceptions::handle_exception_map(resp);
      return;
    }
    peak_store->log(LogLevel::INFO,
                    "API: clearVertices completed successfully");
  }

  // Helper method to call clearEdges from PeakStore
  void clearEdges() {
    peak_store->log(LogLevel::INFO, "API: Entering clearEdges");
    auto resp = peak_store->clearEdges();
    if (!resp.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in clearEdges");
      Exceptions::handle_exception_map(resp);
      return;
    }
    peak_store->log(LogLevel::INFO, "API: clearEdges completed successfully");
  }

  bool hasVertex(const VertexType &v) { return peak_store->hasVertex(v); }
  template <typename E = EdgeType>
  auto addEdge(const VertexType &src, const VertexType &dest)
      -> std::enable_if_t<CinderPeak::Traits::is_unweighted_v<E>,
                          UnweightedEdgeAddResult> {
    peak_store->log(LogLevel::INFO, "API: Entering addEdge (unweighted)");
    auto resp = peak_store->addEdge(src, dest);
    if (!resp.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in addEdge (unweighted)");
      Exceptions::handle_exception_map(resp);
      return {{src, dest}, false};
    }
    peak_store->log(LogLevel::INFO,
                    "API: addEdge (unweighted) completed successfully");
    return {{src, dest}, true};
  }

  template <typename E = EdgeType>
  auto addEdge(const VertexType &src, const VertexType &dest,
               const EdgeType &weight)
      -> std::enable_if_t<!CinderPeak::Traits::is_unweighted_v<E>,
                          WeightedEdgeAddResult> {
    peak_store->log(LogLevel::INFO, "API: Entering addEdge (weighted)");
    auto resp = peak_store->addEdge(src, dest, weight);
    if (!resp.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in addEdge (weighted)");
      Exceptions::handle_exception_map(resp);
      return {{src, dest, weight}, false};
    }
    peak_store->log(LogLevel::INFO,
                    "API: addEdge (weighted) completed successfully");
    return {{src, dest, weight}, true};
  }

  template <typename E = EdgeType>
  auto updateEdge(const VertexType &src, const VertexType &dest,
                  const EdgeType &newWeight)
      -> std::enable_if_t<CinderPeak::Traits::is_weighted_v<E>,
                          UpdateEdgeResult> {
    peak_store->log(LogLevel::INFO, "API: Entering updateEdge");
    auto [status, updatedEdge] = peak_store->updateEdge(src, dest, newWeight);
    if (!status.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in updateEdge");
      Exceptions::handle_exception_map(status);
      return {newWeight, false};
    }
    peak_store->log(LogLevel::INFO, "API: updateEdge completed successfully");
    return {newWeight, true};
  }
  GetEdgeResult getEdge(const VertexType &src, const VertexType &dest) {
    auto [data, status] = peak_store->getEdge(src, dest);
    if (!status.isOK()) {
      Exceptions::handle_exception_map(status);
      return {std::nullopt, false};
    }
    return {std::make_optional(data), true};
  }
  Algorithms::BFSResult<VertexType> bfs(const VertexType &src) {
    return peak_store->bfs(src);
  }

  template <typename V = VertexType, typename E = EdgeType>
  auto toDot(const std::string &filename)
      -> std::enable_if_t<Traits::isTypePrimitive<V>() &&
                          (Traits::isTypePrimitive<E>() ||
                           Traits::is_unweighted_v<E>)> {
    peak_store->toDot(filename);
  }

  std::string getGraphStatistics() { return peak_store->getGraphStatistics(); }
  size_t numEdges() const { return peak_store->numEdges(); }
  size_t numVertices() const { return peak_store->numVertices(); }
  void setConsoleLogging(bool toggle) { peak_store->setConsoleLogging(toggle); }
  void setThrowExceptions(bool toggle) {
    peak_store->setThrowExceptions(toggle);
  }
  void setFileLogging(const std::string &path) {
    peak_store->setFileLogging(path);
  }
  void unsetFileLogging() { peak_store->unsetFileLogging(); }

  CinderGraphRowProxy<VertexType, EdgeType> operator[](const VertexType &v) {
    return CinderGraphRowProxy<VertexType, EdgeType>(*this, v);
  }
  const CinderGraphRowProxy<VertexType, EdgeType>
  operator[](const VertexType &v) const {
    return CinderGraphRowProxy<VertexType, EdgeType>(
        const_cast<CinderGraph<VertexType, EdgeType> &>(*this), v);
  }
};

} // namespace CinderPeak

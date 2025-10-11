#pragma once
#include "Concepts.hpp"
#include "PeakStore.hpp"
#include "PolicyConfiguration.hpp"
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

  // Handle g["A"]["B"]
  EdgeType operator[](const VertexType &dest) const {
    auto [optWeight, found] = graph.getEdge(src, dest);
    if (!found || !optWeight.has_value()) {
      throw std::runtime_error("Edge not found: " + src + " -> " + dest);
    }
    return *optWeight;
  }

  // Allow assignment g["A"]["B"] = 5.0
  CinderGraphRowProxy &operator=(const EdgeType &newWeight) = delete;

  // Support weighted edge insertion/update
  CinderGraphRowProxy &operator()(const VertexType &dest,
                                  const EdgeType &weight) {
    graph.addEdge(src, dest, weight);
    return *this;
  }

  // Alternatively, use assignment-style syntax:
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
                  GraphCreationOptions::getDefaultCreateOptions(),
              const PolicyConfiguration &cfg = PolicyConfiguration()) {
    PeakStore::GraphInternalMetadata metadata(
        "cinder_graph", Traits::isTypePrimitive<VertexType>(),
        Traits::isTypePrimitive<EdgeType>(),
        Traits::isGraphWeighted<EdgeType>(),
        !Traits::isGraphWeighted<EdgeType>());

    peak_store = std::make_unique<PeakStore::PeakStore<VertexType, EdgeType>>(
        metadata, options, cfg);
  }
  VertexAddResult addVertex(const VertexType &v) {
    auto resp = peak_store->addVertex(v);
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      // If exceptions are disabled, handle_exception_map returns -> fallthrough
      return {v, false};
    }
    return {v, true};
  }
  bool removeVertex(const VertexType &v) {
    auto resp = peak_store->removeVertex(v);
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      return false;
    }
    return true;
  }
  RemoveEdgeResult removeEdge(const VertexType &src, const VertexType &dest) {
    auto [data, status] = peak_store->removeEdge(src, dest);
    if (!status.isOK()) {
      Exceptions::handle_exception_map(status);
      return {std::nullopt, false};
    }
    return {std::make_optional(data), true};
  }

  // Helper method to call clearVertices from PeakStore
  void clearVertices() {
    auto resp = peak_store->clearVertices();
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      return;
    }
  }

  // Helper method to call clearEdges from PeakStore
  void clearEdges() {
    auto resp = peak_store->clearEdges();
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      return;
    }
  }

  bool hasVertex(const VertexType &v) { return peak_store->hasVertex(v); }
  template <typename E = EdgeType>
  auto addEdge(const VertexType &src, const VertexType &dest)
      -> std::enable_if_t<CinderPeak::Traits::is_unweighted_v<E>,
                          UnweightedEdgeAddResult> {
    auto resp = peak_store->addEdge(src, dest);
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      return {{src, dest}, false};
    }
    return {{src, dest}, true};
  }

  template <typename E = EdgeType>
  auto addEdge(const VertexType &src, const VertexType &dest,
               const EdgeType &weight)
      -> std::enable_if_t<!CinderPeak::Traits::is_unweighted_v<E>,
                          WeightedEdgeAddResult> {
    auto resp = peak_store->addEdge(src, dest, weight);
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      return {{src, dest, weight}, false};
    }
    return {{src, dest, weight}, true};
  }

  template <typename E = EdgeType>
  auto updateEdge(const VertexType &src, const VertexType &dest,
                  const EdgeType &newWeight)
      -> std::enable_if_t<CinderPeak::Traits::is_weighted_v<E>,
                          UpdateEdgeResult> {

    auto [status, updatedEdge] = peak_store->updateEdge(src, dest, newWeight);

    if (!status.isOK()) {
      Exceptions::handle_exception_map(status);
      return {newWeight, false}; // still return attempted new weight
    }

    return {newWeight, true};
  }
  GetEdgeResult getEdge(const VertexType &src, const VertexType &dest) {
    LOG_INFO("Called getEdge");
    auto [data, status] = peak_store->getEdge(src, dest);
    if (!status.isOK()) {
      Exceptions::handle_exception_map(status);
      return {std::nullopt, false};
    }
    return {std::make_optional(data), true};
  }
  std::string getGraphStatistics() { return peak_store->getGraphStatistics(); }
  size_t numEdges() const { return peak_store->numEdges(); }
  size_t numVertices() const { return peak_store->numVertices(); }

  static void setConsoleLogging(const bool toggle) {
    CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>::setConsoleLogging(
        toggle);
  }
  // ===============================
  // Add this section before the closing brace of CinderGraph
  // ===============================
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
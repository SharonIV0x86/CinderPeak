#pragma once
#include "Concepts.hpp"
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
// class CinderGraph;

template <typename VertexType, typename EdgeType> class GraphList {
private:
  std::unique_ptr<CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>
      peak_store;

  using Vertex_t = VertexType;
  using Edge_t = EdgeType;
  using EdgeKey = std::pair<Vertex_t, Vertex_t>;
  using WeightedEdgeKey = std::tuple<Vertex_t, Vertex_t, Edge_t>;

  // Insert / result types (Boost-like)
  using VertexAddResult = std::pair<Vertex_t, bool>;
  using UnweightedEdgeAddResult = std::pair<EdgeKey, bool>;
  using WeightedEdgeAddResult = std::pair<WeightedEdgeKey, bool>;

  // UpdateEdgeResult: {previousWeight, updatedFlag}
  // (previousWeight is Edge_t{} when edge missing or unknown)
  using UpdateEdgeResult = std::pair<Edge_t, bool>;

  // GetEdgeResult: {optional(weight), foundFlag}
  // Note: optional already conveys presence; bool duplicates that info but kept
  // per typedef.
  using GetEdgeResult = std::pair<std::optional<Edge_t>, bool>;

public:
  GraphList(const GraphCreationOptions &options =
                GraphCreationOptions::getDefaultCreateOptions(),
            const PolicyConfiguration &cfg = PolicyConfiguration()) {
    PeakStore::GraphInternalMetadata metadata(
        "graph_list", Traits::isTypePrimitive<VertexType>(),
        Traits::isTypePrimitive<EdgeType>(),
        Traits::isGraphWeighted<EdgeType>(),
        !Traits::isGraphWeighted<EdgeType>());

    peak_store = std::make_unique<PeakStore::PeakStore<VertexType, EdgeType>>(
        metadata, options, cfg);
  }

  VertexAddResult addVertex(const Vertex_t &v) {
    auto resp = peak_store->addVertex(v);
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      // If exceptions are disabled, handle_exception_map returns -> fallthrough
      return {v, false};
    }
    return {v, true};
  }
  bool removeVertex(const Vertex_t &v) {
    auto resp = peak_store->removeVertex(v);
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      return false;
    }
    return true;
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
  template <typename E = Edge_t>
  auto addEdge(const Vertex_t &src, const Vertex_t &dest)
      -> std::enable_if_t<CinderPeak::Traits::is_unweighted_v<E>,
                          UnweightedEdgeAddResult> {
    auto resp = peak_store->addEdge(src, dest);
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      return {{src, dest}, false};
    }
    return {{src, dest}, true};
  }

  template <typename E = Edge_t>
  auto addEdge(const Vertex_t &src, const Vertex_t &dest, const Edge_t &weight)
      -> std::enable_if_t<!CinderPeak::Traits::is_unweighted_v<E>,
                          WeightedEdgeAddResult> {
    auto resp = peak_store->addEdge(src, dest, weight);
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      return {{src, dest, weight}, false};
    }
    return {{src, dest, weight}, true};
  }

  template <typename E = Edge_t>
  auto updateEdge(const Vertex_t &src, const Vertex_t &dest,
                  const Edge_t &newWeight)
      -> std::enable_if_t<CinderPeak::Traits::is_weighted_v<E>,
                          UpdateEdgeResult> {

    auto [status, updatedEdge] = peak_store->updateEdge(src, dest, newWeight);

    if (!status.isOK()) {
      Exceptions::handle_exception_map(status);
      return {newWeight, false}; // still return attempted new weight
    }

    return {newWeight, true};
  }
  GetEdgeResult getEdge(const Vertex_t &src, const Vertex_t &dest) {
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
};

} // namespace CinderPeak
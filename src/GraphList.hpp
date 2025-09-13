#pragma once
#include "Concepts.hpp"
#include "StorageEngine/Utils.hpp"
#include <iostream>
#include <optional>
#include <tuple>
#include <utility>

namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class PeakStore;
}
class CinderGraph;

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
                CinderPeak::GraphCreationOptions::getDefaultCreateOptions()) {
    CinderPeak::PeakStore::GraphInternalMetadata metadata(
        "graph_list", CinderPeak::Traits::isTypePrimitive<VertexType>(),
        CinderPeak::Traits::isTypePrimitive<EdgeType>(),
        CinderPeak::Traits::isGraphWeighted<EdgeType>(),
        !CinderPeak::Traits::isGraphWeighted<EdgeType>());
    peak_store = std::make_unique<
        CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>(metadata,
                                                                options);
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

    auto [prevValue, prevStatus] = peak_store->getEdge(src, dest);
    if (!prevStatus.isOK()) {
      Exceptions::handle_exception_map(prevStatus);
      return {Edge_t(), false};
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
  size_t numEdges() const { return peak_store->numEdges(); }
  size_t numVertices() const { return peak_store->numVertices(); }

  static void setConsoleLogging(const bool toggle) {
    CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>::setConsoleLogging(
        toggle);
  }

  void visualize() {
    LOG_INFO("Called GraphList:visualize");
    peak_store->visualize();
  }
};

} // namespace CinderPeak
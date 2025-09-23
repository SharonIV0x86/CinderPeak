#pragma once
#include "Concepts.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/Utils.hpp"
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>

namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class PeakStore;
}
// class CinderGraph;

template <typename VertexType, typename EdgeType> class GraphMatrix;

template <typename VertexType, typename EdgeType> class EdgeAccessor {
private:
  GraphMatrix<VertexType, EdgeType> &graph;
  const VertexType &src;

public:
  EdgeAccessor(GraphMatrix<VertexType, EdgeType> &g, const VertexType &s)
      : graph(g), src(s) {}

  class EdgeReference {
  private:
    GraphMatrix<VertexType, EdgeType> &graph;
    VertexType src, dest;

  public:
    EdgeReference(GraphMatrix<VertexType, EdgeType> &g, const VertexType &s,
                  const VertexType &d)
        : graph(g), src(s), dest(d) {}

    EdgeReference &operator=(const EdgeType &weight) {
      graph.addEdge(src, dest, weight);
      return *this;
    }

    operator std::optional<EdgeType>() const {
      return graph.getEdge(src, dest);
    }
  };

  EdgeReference operator[](const VertexType &dest) {
    return EdgeReference(graph, src, dest);
  }

  std::optional<EdgeType> operator[](const VertexType &dest) const {
    return graph.getEdge(src, dest);
  }
};

template <typename VertexType, typename EdgeType> class GraphMatrix {
private:
  std::unique_ptr<CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>
      peak_store;

public:
  // Aliases for readability
  using Vertex_t = VertexType;
  using Edge_t = EdgeType;
  using VertexAddResult = std::pair<Vertex_t, bool>;
  using UnweightedEdgeAddResult =
      std::pair<std::pair<Vertex_t, Vertex_t>, bool>;
  using WeightedEdgeAddResult =
      std::pair<std::tuple<Vertex_t, Vertex_t, Edge_t>, bool>;
  using UpdateEdgeResult = std::pair<std::optional<Edge_t>, bool>;
  using GetEdgeResult = std::pair<std::optional<Edge_t>, bool>;

  GraphMatrix(const GraphCreationOptions &options =
                  GraphCreationOptions::getDefaultCreateOptions(),
              const PolicyConfiguration &cfg = PolicyConfiguration()) {
    PeakStore::GraphInternalMetadata metadata(
        "graph_matrix", Traits::isTypePrimitive<VertexType>(),
        Traits::isTypePrimitive<EdgeType>(),
        Traits::isGraphWeighted<EdgeType>(),
        !Traits::isGraphWeighted<EdgeType>());
    peak_store = std::make_unique<PeakStore::PeakStore<VertexType, EdgeType>>(
        metadata, options, cfg);
  }

  VertexAddResult addVertex(const VertexType &src) {
    auto resp = peak_store->addVertex(src);
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      return {src, false};
    }
    return {src, true};
  }

  bool removeVertex(const VertexType &src) {
    auto resp = peak_store->removeVertex(src);
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      return false;
    }
    return true;
  }

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
      -> std::enable_if_t<CinderPeak::Traits::is_weighted_v<E>,
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
    auto resp = peak_store->updateEdge(src, dest, newWeight);
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      return {std::nullopt, false};
    }
    return {newWeight, true};
  }

  GetEdgeResult getEdge(const VertexType &src, const VertexType &dest) const {
    auto [data, status] = peak_store->getEdge(src, dest);
    if (!status.isOK()) {
      Exceptions::handle_exception_map(status);
      return {std::nullopt, false};
    }
    return {data, true};
  }
  bool hasVertex(const VertexType &src) {
    PeakStatus status = peak_store->hasVertex(src);
    if (!status.isOK()) {
      Exceptions::handle_exception_map(status);
      return false;
    }
    return true;
  }
  std::string getGraphStatistics() { return peak_store->getGraphStatistics(); }

  static void setConsoleLogging(const bool toggle) {
    CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>::setConsoleLogging(
        toggle);
  }

  EdgeAccessor<VertexType, EdgeType> operator[](const VertexType &src) {
    return EdgeAccessor<VertexType, EdgeType>(*this, src);
  }

  const EdgeAccessor<VertexType, EdgeType>
  operator[](const VertexType &src) const {
    return EdgeAccessor<VertexType, EdgeType>(const_cast<GraphMatrix &>(*this),
                                              src);
  }

  friend class EdgeAccessor<VertexType, EdgeType>;
};

} // namespace CinderPeak

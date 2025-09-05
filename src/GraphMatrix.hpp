#pragma once
#include "Concepts.hpp"
#include "StorageEngine/Utils.hpp"
#include <iostream>
#include <memory>
#include <type_traits>
namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class PeakStore;
}
class CinderGraph;

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
    operator EdgeType() const { return graph.getEdge(src, dest); }
  };
  EdgeReference operator[](const VertexType &dest) {
    return EdgeReference(graph, src, dest);
  }
  EdgeType operator[](const VertexType &dest) const {
    return graph.getEdge(src, dest);
  }
};

template <typename VertexType, typename EdgeType> class GraphMatrix {
private:
  std::unique_ptr<CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>
      peak_store;

public:
  GraphMatrix(const GraphCreationOptions &options =
                  CinderPeak::GraphCreationOptions::getDefaultCreateOptions()) {
    CinderPeak::PeakStore::GraphInternalMetadata metadata(
        "graph_matrix", CinderPeak::Traits::isTypePrimitive<VertexType>(),
        CinderPeak::Traits::isTypePrimitive<EdgeType>(),
        CinderPeak::Traits::isGraphWeighted<EdgeType>(),
        !CinderPeak::Traits::isGraphWeighted<EdgeType>());
    peak_store = std::make_unique<
        CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>(metadata,
                                                                options);
  }

  void addVertex(const VertexType &src) {
    auto resp = peak_store->addVertex(src);
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      return;
    }
  }
  template <typename E = EdgeType>
  auto addEdge(const VertexType &src, const VertexType &dest)
      -> std::enable_if_t<CinderPeak::Traits::is_unweighted_v<E>, void> {
    auto resp = peak_store->addEdge(src, dest);
    if (!resp.isOK())
      Exceptions::handle_exception_map(resp);
  }
  template <typename E = EdgeType>
  auto addEdge(const VertexType &src, const VertexType &dest,
               const EdgeType &weight)
      -> std::enable_if_t<CinderPeak::Traits::is_weighted_v<E>, void> {
    auto resp = peak_store->addEdge(src, dest, weight);
    if (!resp.isOK())
      Exceptions::handle_exception_map(resp);
  }

  EdgeType getEdge(const VertexType &src, const VertexType &dest) const {
    auto [data, status] = peak_store->getEdge(src, dest);
    if (!status.isOK()) {
      Exceptions::handle_exception_map(status);
      return EdgeType();
    }
    return data;
  }
  void visualize() { LOG_INFO("Called GraphMatrix:visualize"); }

  // Helper method to call getGraphStatistics() from Peakstore
  std::string getGraphStatistics() { return peak_store->getGraphStatistics(); }

  // Helper method to call setConsoleLogging function from Peakstore
  static void setConsoleLogging(const bool toggle) {
    CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>::setConsoleLogging(toggle);
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

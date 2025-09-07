#pragma once
#include "Concepts.hpp"
#include "StorageEngine/Utils.hpp"
#include <iostream>
#include <memory>
namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class PeakStore;
}
struct GraphCreationOptions {
    enum Type { Undirected, Directed };
    GraphCreationOptions(std::initializer_list<Type> t) {}
    static GraphCreationOptions getDefaultCreateOptions() { return GraphCreationOptions({Undirected}); }
};
namespace Traits {
    template<typename T> constexpr bool is_unweighted_v = false;
    template<typename T> constexpr bool is_weighted_v = true;
    template<typename T> constexpr bool isTypePrimitive() { return std::is_fundamental<T>::value; }
    template<typename T> constexpr bool isGraphWeighted() { return true; }
}

template <typename VertexType, typename EdgeType>
class GraphList {
private:
  std::unique_ptr<CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>
      peak_store;

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

  void addVertex(const VertexType &v) {
    auto resp = peak_store->addVertex(v);
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
      -> std::enable_if_t<!CinderPeak::Traits::is_unweighted_v<E>, void> {
    auto resp = peak_store->addEdge(src, dest, weight);
    if (!resp.isOK())
      Exceptions::handle_exception_map(resp);
  }

  template <typename E = EdgeType>
  auto updateEdge(const VertexType &src, const VertexType &dest,
               const EdgeType &newWeight)
    -> std::enable_if_t<CinderPeak::Traits::is_weighted_v<E>, bool> {
    auto resp = peak_store->updateEdge(src, dest, newWeight);
    if (!resp.isOK())
      return false;
    return true;
  }

  EdgeType getEdge(const VertexType &src, const VertexType &dest) {
    LOG_INFO("Called getEdge");
    auto [data, status] = peak_store->getEdge(src, dest);
    if (!status.isOK()) {
      Exceptions::handle_exception_map(status);
      return EdgeType(); 
    }
    return data;
  }

  size_t numEdges() const {
    return peak_store->numEdges();
  }

  static void setConsoleLogging(const bool toggle) {
    CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>::setConsoleLogging(toggle);
  }

  void visualize() {
    LOG_INFO("Called GraphList:visualize");
    peak_store->visualize();
  }

  void clearEdges() {
    auto resp = peak_store->clearEdges();
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
    }
  }
};

} // namespace CinderPeak

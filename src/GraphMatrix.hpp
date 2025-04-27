#include "StorageEngine/Utils.hpp"
#include <iostream>

namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class PeakStore;
}

template <typename VertexType, typename EdgeType> class GraphMatrix {
private:
  std::unique_ptr<CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>
      peak_store;

public:
  GraphMatrix(const GraphCreationOptions &options =
                  CinderPeak::PeakStore::DEFAULT_GRAPH_OPTIONS) {
    CinderPeak::PeakStore::GraphInternalMetadata metadata("graph_matrix", isTypePrimitive<VertexType>(), isTypePrimitive<EdgeType>());
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
  void addEdge(const VertexType &src, const VertexType &dest) {
    auto ctx = peak_store->getContext();
    if(ctx->create_options->hasOption(GraphCreationOptions::Weighted)){
      LOG_CRITICAL("Cannot call unweighted addEdge on a weighted graph, missing weight");
      //throw peroper exceptionc
      return;
    }
    auto resp = peak_store->addEdge(src, dest);
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
      return;
    }
    return ;
  }
  void addEdge(const VertexType &src, const VertexType &dest,
               const EdgeType &weight) {
    auto ctx = peak_store->getContext();
    if(ctx->create_options->hasOption(GraphCreationOptions::Unweighted)){
      LOG_CRITICAL("Cannot call weighted addEdge on a unweighted graph, extra weight");
      //throw proper exception
    }
    auto resp = peak_store->addEdge(src, dest, weight);
    if (!resp.isOK()) {
      LOG_INFO("Called weighted addEdge");
      Exceptions::handle_exception_map(resp);
      return;
    }
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
};
} // namespace CinderPeak

#pragma once
#include "StorageEngine/Utils.hpp"
#include <iostream>
namespace CinderPeak
{
  namespace PeakStore
  {
    template <typename VertexType, typename EdgeType>
    class PeakStore;
  }
  class CinderGraph;
  template <typename VertexType, typename EdgeType>
  class GraphList
  {
  private:
    std::unique_ptr<CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>
        peak_store;

  public:
    GraphList(const GraphCreationOptions &options = CinderPeak::GraphCreationOptions::getDefaultCreateOptions())
    {
      CinderPeak::PeakStore::GraphInternalMetadata metadata(
          "graph_list", isTypePrimitive<VertexType>(),
          isTypePrimitive<EdgeType>());
      peak_store = std::make_unique<
          CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>(metadata,
                                                                  options);
    }

    void addVertex(const VertexType &v)
    {
      auto resp = peak_store->addVertex(v);
      if (!resp.isOK())
      {
        Exceptions::handle_exception_map(resp);
        return;
      }
    }

    void addEdge(const VertexType &src, const VertexType &dest)
    {
      auto ctx = peak_store->getContext();
      if (ctx->create_options->hasOption(GraphCreationOptions::Weighted))
      {
        LOG_CRITICAL(
            "Cannot call unweighted addEdge on a weighted graph, missing weight");
        return;
      }
      auto resp = peak_store->addEdge(src, dest);
      if (!resp.isOK())
      {
        Exceptions::handle_exception_map(resp);
        return;
      }
    }

    void addEdge(const VertexType &src, const VertexType &dest,
                 const EdgeType &weight)
    {
      auto ctx = peak_store->getContext();
      if (ctx->create_options->hasOption(GraphCreationOptions::Unweighted))
      {
        LOG_CRITICAL(
            "Cannot call weighted addEdge on an unweighted graph, extra weight");
        return;
      }
      auto resp = peak_store->addEdge(src, dest, weight);
      if (!resp.isOK())
      {
        LOG_INFO("Called weighted addEdge");
        Exceptions::handle_exception_map(resp);
        return;
      }
    }

    EdgeType getEdge(const VertexType &src, const VertexType &dest)
    {
      LOG_INFO("Called getEdge");
      auto [data, status] = peak_store->getEdge(src, dest);
      if (!status.isOK())
      {
        Exceptions::handle_exception_map(status);
        return EdgeType(); // Return default-constructed EdgeType on error
      }
      return data;
    }

    void visualize()
    {
      LOG_INFO("Called GraphList:visualize");
      peak_store->visualize();
    }
  };

} // namespace CinderPeak

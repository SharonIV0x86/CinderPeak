#include "StorageEngine/Utils.hpp"
#include <iostream>
#include <memory>

namespace CinderPeak
{
  namespace PeakStore
  {
    template <typename VertexType, typename EdgeType>
    class PeakStore;
  }

  template <typename VertexType, typename EdgeType>
  class GraphMatrix;

  template <typename VertexType, typename EdgeType>
  class EdgeAccessor
  {
  private:
    GraphMatrix<VertexType, EdgeType> &graph;
    const VertexType &src;

  public:
    EdgeAccessor(GraphMatrix<VertexType, EdgeType> &g, const VertexType &s)
        : graph(g), src(s) {}

    class EdgeReference
    {
    private:
      GraphMatrix<VertexType, EdgeType> &graph;
      VertexType src, dest;

    public:
      EdgeReference(GraphMatrix<VertexType, EdgeType> &g,
                    const VertexType &s, const VertexType &d)
          : graph(g), src(s), dest(d) {}

      EdgeReference &operator=(const EdgeType &weight)
      {
        graph.addEdge(src, dest, weight);
        return *this;
      }
      operator EdgeType() const
      {
        return graph.getEdge(src, dest);
      }
    };
    EdgeReference operator[](const VertexType &dest)
    {
      return EdgeReference(graph, src, dest);
    }
    EdgeType operator[](const VertexType &dest) const
    {
      return graph.getEdge(src, dest);
    }
  };

  template <typename VertexType, typename EdgeType>
  class GraphMatrix
  {
  private:
    std::unique_ptr<CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>> peak_store;

  public:
    GraphMatrix(const GraphCreationOptions &options =
                    CinderPeak::PeakStore::DEFAULT_GRAPH_OPTIONS)
    {
      CinderPeak::PeakStore::GraphInternalMetadata metadata(
          "graph_matrix", isTypePrimitive<VertexType>(),
          isTypePrimitive<EdgeType>());
      peak_store = std::make_unique<
          CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>(metadata,
                                                                  options);
    }

    void addVertex(const VertexType &src)
    {
      auto resp = peak_store->addVertex(src);
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
            "Cannot call weighted addEdge on a unweighted graph, extra weight");
        return;
      }
      auto resp = peak_store->addEdge(src, dest, weight);
      if (!resp.isOK())
      {
        Exceptions::handle_exception_map(resp);
        return;
      }
    }

    EdgeType getEdge(const VertexType &src, const VertexType &dest) const
    {
      auto [data, status] = peak_store->getEdge(src, dest);
      if (!status.isOK())
      {
        Exceptions::handle_exception_map(status);
        return EdgeType();
      }
      return data;
    }

    void visualize()
    {
      LOG_INFO("Called GraphMatrix:visualize");
      peak_store->visualize();
    }

    EdgeAccessor<VertexType, EdgeType> operator[](const VertexType &src)
    {
      return EdgeAccessor<VertexType, EdgeType>(*this, src);
    }
    const EdgeAccessor<VertexType, EdgeType> operator[](const VertexType &src) const
    {
      return EdgeAccessor<VertexType, EdgeType>(const_cast<GraphMatrix &>(*this), src);
    }
    friend class EdgeAccessor<VertexType, EdgeType>;
  };

} // namespace CinderPeak

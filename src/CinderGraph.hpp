#pragma once
#include "Algorithms/CinderPeakAlgorithms.hpp"
#include "Concepts.hpp"
#include "PeakStore.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/GraphStatistics.hpp"
#include "StorageEngine/Utils.hpp"
#include "StorageEngine/GraphSerialization.hpp"
#include <iostream>
#include <optional>
#include <tuple>
#include <utility>

namespace CinderPeak
{
  namespace PeakStore
  {
    template <typename VertexType, typename EdgeType>
    class PeakStore;
  } // namespace PeakStore

  namespace Serialization
  {
    template <typename VertexType, typename EdgeType>
    class GraphSerializer;
  } // namespace Serialization

  template <typename VertexType, typename EdgeType>
  class CinderGraph;

  // Proxy for a graph row: allows g[src][dest] access and assignment
  template <typename VertexType, typename EdgeType>
  class CinderGraphRowProxy
  {
    CinderGraph<VertexType, EdgeType> &graph;
    VertexType src;

  public:
    CinderGraphRowProxy(CinderGraph<VertexType, EdgeType> &g, const VertexType &s)
        : graph(g), src(s) {}

    // Read-only access: returns EdgeType for value-like access
    EdgeType operator[](const VertexType &dest) const
    {
      auto [optWeight, found] = graph.getEdge(src, dest);
      if (!found || !optWeight.has_value())
      {
        throw std::runtime_error("Edge not found");
      }
      return *optWeight;
    }

    CinderGraphRowProxy &operator=(const EdgeType &newWeight) = delete;

    // Call operator for addEdge-style: g[src](dest, weight)
    CinderGraphRowProxy &operator()(const VertexType &dest, const EdgeType &weight)
    {
      graph.addEdge(src, dest, weight);
      return *this;
    }

    // Helper proxy for assignment: g[src][dest] = weight
    struct EdgeAssignProxy
    {
      CinderGraph<VertexType, EdgeType> &graph;
      VertexType src, dest;

      EdgeAssignProxy(CinderGraph<VertexType, EdgeType> &g, const VertexType &s,
                      const VertexType &d)
          : graph(g), src(s), dest(d) {}

      EdgeAssignProxy &operator=(const EdgeType &weight)
      {
        graph.addEdge(src, dest, weight);
        return *this;
      }

      operator EdgeType() const
      {
        auto [optWeight, found] = graph.getEdge(src, dest);
        return (found && optWeight) ? *optWeight : EdgeType{};
      }
    };

    EdgeAssignProxy operator[](const VertexType &dest)
    {
      return EdgeAssignProxy(graph, src, dest);
    }
  };

  template <typename VertexType, typename EdgeType>
  class CinderGraph
  {
    std::unique_ptr<PeakStore::PeakStore<VertexType, EdgeType>> peak_store;

    using EdgeKey = std::pair<VertexType, VertexType>;
    using WeightedEdgeKey = std::tuple<VertexType, VertexType, EdgeType>;

    using VertexAddResult = std::pair<VertexType, bool>;
    using UnweightedEdgeAddResult = std::pair<EdgeKey, bool>;
    using WeightedEdgeAddResult = std::pair<WeightedEdgeKey, bool>;
    using UpdateEdgeResult = std::pair<EdgeType, bool>;
    using GetEdgeResult = std::pair<std::optional<EdgeType>, bool>;
    using RemoveEdgeResult = std::pair<std::optional<EdgeType>, bool>;

  public:
    CinderGraph(const GraphCreationOptions &options =
                    GraphCreationOptions::getDefaultCreateOptions(),
                const PolicyConfiguration &cfg = PolicyConfiguration())
    {
      PeakStore::GraphInternalMetadata metadata(
          "cinder_graph", Traits::isTypePrimitive<VertexType>(),
          Traits::isTypePrimitive<EdgeType>(), Traits::isGraphWeighted<EdgeType>(),
          !Traits::isGraphWeighted<EdgeType>());

      peak_store = std::make_unique<PeakStore::PeakStore<VertexType, EdgeType>>(metadata, options, cfg);
    }

    VertexAddResult addVertex(const VertexType &v)
    {
      auto resp = peak_store->addVertex(v);
      if (!resp.isOK())
      {
        Exceptions::handle_exception_map(resp);
        return {v, false};
      }
      return {v, true};
    }

    bool removeVertex(const VertexType &v)
    {
      auto resp = peak_store->removeVertex(v);
      if (!resp.isOK())
      {
        Exceptions::handle_exception_map(resp);
        return false;
      }
      return true;
    }

    RemoveEdgeResult removeEdge(const VertexType &src, const VertexType &dest)
    {
      auto [data, status] = peak_store->removeEdge(src, dest);
      if (!status.isOK())
      {
        Exceptions::handle_exception_map(status);
        return {std::nullopt, false};
      }
      return {std::make_optional(data), true};
    }

    void clearVertices()
    {
      auto resp = peak_store->clearVertices();
      if (!resp.isOK())
      {
        Exceptions::handle_exception_map(resp);
        return;
      }
    }

    void clearEdges()
    {
      auto resp = peak_store->clearEdges();
      if (!resp.isOK())
      {
        Exceptions::handle_exception_map(resp);
        return;
      }
    }

    bool hasVertex(const VertexType &v) { return peak_store->hasVertex(v); }

    template <typename E = EdgeType>
    auto addEdge(const VertexType &src, const VertexType &dest)
        -> std::enable_if_t<Traits::is_unweighted_v<E>, UnweightedEdgeAddResult>
    {
      auto resp = peak_store->addEdge(src, dest);
      if (!resp.isOK())
      {
        Exceptions::handle_exception_map(resp);
        return {{src, dest}, false};
      }
      return {{src, dest}, true};
    }

    template <typename E = EdgeType>
    auto addEdge(const VertexType &src, const VertexType &dest, const EdgeType &weight)
        -> std::enable_if_t<!Traits::is_unweighted_v<E>, WeightedEdgeAddResult>
    {
      auto resp = peak_store->addEdge(src, dest, weight);
      if (!resp.isOK())
      {
        Exceptions::handle_exception_map(resp);
        return {{src, dest, weight}, false};
      }
      return {{src, dest, weight}, true};
    }

    template <typename E = EdgeType>
    auto updateEdge(const VertexType &src, const VertexType &dest, const EdgeType &newWeight)
        -> std::enable_if_t<Traits::is_weighted_v<E>, UpdateEdgeResult>
    {
      auto [status, updatedEdge] = peak_store->updateEdge(src, dest, newWeight);
      if (!status.isOK())
      {
        Exceptions::handle_exception_map(status);
        return {newWeight, false};
      }
      return {newWeight, true};
    }

    GetEdgeResult getEdge(const VertexType &src, const VertexType &dest)
    {
      LOG_INFO("Called getEdge");
      auto [data, status] = peak_store->getEdge(src, dest);
      if (!status.isOK())
      {
        Exceptions::handle_exception_map(status);
        return {std::nullopt, false};
      }
      return {std::make_optional(data), true};
    }

    std::string getGraphStatistics() { return peak_store->getGraphStatistics(); }
    size_t numEdges() const { return peak_store->numEdges(); }
    size_t numVertices() const { return peak_store->numVertices(); }

    static void setConsoleLogging(const bool toggle)
    {
      PeakStore::PeakStore<VertexType, EdgeType>::setConsoleLogging(toggle);
    }

    CinderGraphRowProxy<VertexType, EdgeType> operator[](const VertexType &v)
    {
      return CinderGraphRowProxy<VertexType, EdgeType>(*this, v);
    }

    const CinderGraphRowProxy<VertexType, EdgeType> operator[](const VertexType &v) const
    {
      return CinderGraphRowProxy<VertexType, EdgeType>(const_cast<CinderGraph<VertexType, EdgeType> &>(*this), v);
    }

    auto save(const std::string &filepath) -> typename Serialization::GraphSerializer<VertexType, EdgeType>::SerializationResult
    {
      auto vertices = peak_store->getAllVertices();
      auto edges = peak_store->getAllEdges();
      auto options = peak_store->getGraphOptions();
      return Serialization::GraphSerializer<VertexType, EdgeType>::serialize(vertices, edges, options, filepath);
    }

    auto load(const std::string &filepath) -> typename Serialization::GraphSerializer<VertexType, EdgeType>::SerializationResult
    {
      std::vector<VertexType> vertices;
      std::vector<std::tuple<VertexType, VertexType, EdgeType>> edges;
      GraphCreationOptions options = GraphCreationOptions::getDefaultCreateOptions();

      auto result = Serialization::GraphSerializer<VertexType, EdgeType>::deserialize(filepath, vertices, edges, options);
      if (!result.success)
        return result;

      clearVertices();
      clearEdges();

      PeakStore::GraphInternalMetadata metadata(
          "cinder_graph", Traits::isTypePrimitive<VertexType>(),
          Traits::isTypePrimitive<EdgeType>(), Traits::isGraphWeighted<EdgeType>(),
          !Traits::isGraphWeighted<EdgeType>());

      peak_store = std::make_unique<PeakStore::PeakStore<VertexType, EdgeType>>(metadata, options, PolicyConfiguration());

      for (const auto &v : vertices)
        addVertex(v);

      for (const auto &edge : edges)
      {
        const auto &src = std::get<0>(edge);
        const auto &dest = std::get<1>(edge);
        const auto &weight = std::get<2>(edge);
        if constexpr (Traits::is_unweighted_v<EdgeType>)
          addEdge(src, dest);
        else
          addEdge(src, dest, weight);
      }

      return result;
    }
  };

} // namespace CinderPeak
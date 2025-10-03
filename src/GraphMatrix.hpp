#pragma once
#include "Concepts.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/GraphStatistics.hpp"
#include "StorageEngine/Utils.hpp"
#include <iostream>
#include <fstream>
#include <optional>
#include <string>
#include <type_traits>
#include <tuple>
#include <utility>
#include <vector>
#include <sstream>

namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class PeakStore;
}

template <typename VertexType, typename EdgeType> class EdgeAccessor;

template <typename VertexType, typename EdgeType>
class GraphMatrix {
private:
  std::unique_ptr<CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>
      peak_store;

public:
  using Vertex_t = VertexType;
  using Edge_t = EdgeType;
  using VertexAddResult = std::pair<Vertex_t, bool>;
  using UnweightedEdgeAddResult = std::pair<std::pair<Vertex_t, Vertex_t>, bool>;
  using WeightedEdgeAddResult = std::pair<std::tuple<Vertex_t, Vertex_t, Edge_t>, bool>;
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
  auto addEdge(const VertexType &src, const VertexType &dest, const EdgeType &weight)
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
      -> std::enable_if_t<CinderPeak::Traits::is_weighted_v<E>, UpdateEdgeResult> {
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

  
  bool saveToFile(const std::string &filename) const {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
      LOG_ERROR("Failed to open file for writing: " + filename);
      return false;
    }

    try {
      // Get adjacency list via peak store context
      const auto &adj_list =
          peak_store->getContext()->adjacency_storage->getAdjList();

      // Write all vertices
      for (const auto &vertex_pair : adj_list) {
        ofs << "V " << vertex_pair.first << "\n";
      }

      // Write edges
      for (const auto &vertex_pair : adj_list) {
        const Vertex_t &src = vertex_pair.first;
        const auto &neighbors = vertex_pair.second;
        for (const auto &neighbor : neighbors) {
          ofs << "E " << src << " " << neighbor.first;
          if constexpr (!Traits::is_unweighted_v<Edge_t>) {
            ofs << " " << neighbor.second;
          }
          ofs << "\n";
        }
      }

      ofs.close();
      return true;
    } catch (const std::exception &e) {
      LOG_ERROR("Exception during saveToFile: " + std::string(e.what()));
      return false;
    }
  }

  
  bool loadFromFile(const std::string &filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
      LOG_ERROR("Failed to open file for reading: " + filename);
      return false;
    }

    try {
      // Clear graph edges and remove all vertices
      clearEdges();

      // Remove all current vertices
      const auto &adj_list = peak_store->getContext()->adjacency_storage->getAdjList();
      std::vector<Vertex_t> vertices_to_remove;
      for (const auto &pair : adj_list) {
        vertices_to_remove.push_back(pair.first);
      }
      for (const auto &v : vertices_to_remove) {
        peak_store->removeVertex(v);
      }

      std::string line;
      while (std::getline(ifs, line)) {
        if (line.empty())
          continue;

        std::istringstream iss(line);
        char type;
        iss >> type;

        if (type == 'V') {
          Vertex_t v;
          iss >> v;
          addVertex(v);
        } else if (type == 'E') {
          Vertex_t src, dest;
          iss >> src >> dest;

          if constexpr (Traits::is_unweighted_v<Edge_t>) {
            addEdge(src, dest);
          } else {
            Edge_t weight;
            iss >> weight;
            addEdge(src, dest, weight);
          }
        }
      }

      ifs.close();
      return true;
    } catch (const std::exception &e) {
      LOG_ERROR("Exception during loadFromFile: " + std::string(e.what()));
      return false;
    }
  }

  // Helper method to clear edges
  void clearEdges() {
    auto resp = peak_store->clearEdges();
    if (!resp.isOK()) {
      Exceptions::handle_exception_map(resp);
    }
  }
};

} // namespace CinderPeak

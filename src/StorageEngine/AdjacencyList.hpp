#pragma once
#include "Concepts.hpp"
#include "StorageEngine/GraphContext.hpp"
#include "Utils.hpp"
#include <memory>
namespace CinderPeak {
template <typename, typename> class PeakStorageInterface;

namespace PeakStore {
template <typename VertexType, typename EdgeType>
class AdjacencyList
    : public CinderPeak::PeakStorageInterface<VertexType, EdgeType> {
private:
  std::unordered_map<VertexType, std::vector<std::pair<VertexType, EdgeType>>,
                     VertexHasher<VertexType>>
      _adj_list;

public:
  AdjacencyList() { LOG_INFO("Initialized Adjacency List object"); }

  const PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest,
                                const EdgeType &weight = EdgeType()) {
    if (auto it = _adj_list.find(src); it == _adj_list.end())
      return PeakStatus::VertexNotFound();
    if (auto it = _adj_list.find(dest); it == _adj_list.end())
      return PeakStatus::VertexNotFound();
    _adj_list[src].emplace_back(dest, weight);
    return PeakStatus::OK();
  }

  // Added method for bulk edges insertion
  // Usage:
  // For unweighted graph: graph.addEdges({ {1, 2}, {2, 3}, {3, 4} });
  // For weighted graph: graph.addEdges({ {1, 2, 5}, {2, 3, 7}, {3, 4, 9} });
  template <typename EdgeContainer>
  const PeakStatus impl_addEdges(const EdgeContainer &edges) {
    PeakStatus peak_status = PeakStatus::OK();

    for (const auto &edge : edges) {
      VertexType src, dest;
      EdgeType weight = EdgeType(); // default weight

      // Extract values based on container type at compile time
      if constexpr (std::is_same_v<typename EdgeContainer::value_type,
                                   std::pair<VertexType, VertexType>>) {
        src = edge.first;
        dest = edge.second;
      } else if constexpr (std::is_same_v<
                               typename EdgeContainer::value_type,
                               std::tuple<VertexType, VertexType, EdgeType>>) {
        src = std::get<0>(edge);
        dest = std::get<1>(edge);
        weight = std::get<2>(edge);
      }

      if (auto it = _adj_list.find(src); it == _adj_list.end()) {
        LOG_WARNING(
            (std::ostringstream() << "The vertex " << src << " does not exist.")
                .str());
        peak_status = PeakStatus::VertexNotFound();
        continue;
      }
      if (auto it = _adj_list.find(dest); it == _adj_list.end()) {
        LOG_WARNING(
            (std::ostringstream() << "The vertex " << src << " does not exist.")
                .str());
        peak_status = PeakStatus::VertexNotFound();
        continue;
      }

      _adj_list[src].emplace_back(dest, weight);
    }

    return peak_status;
  }

  const PeakStatus impl_addVertex(const VertexType &src) override {
    if constexpr (CinderPeak::Traits::is_primitive_or_string_v<VertexType>) {
      if (auto it = _adj_list.find(src); it != _adj_list.end()) {
        LOG_WARNING("Vertex already exists with primitive type");
        return PeakStatus::VertexAlreadyExists(
            "Primitive Vertex Already Exists");
      }
      LOG_DEBUG("Unmatched vertices");
      LOG_INFO("Inside primitive block");
    } else {
      if (auto it = _adj_list.find(src); it != _adj_list.end()) {
        const VertexType &existingVertex = it->first;
        if (existingVertex.__id_ == src.__id_) {
          LOG_DEBUG("Matching vertex IDs");
          return PeakStatus::VertexAlreadyExists(
              "Non Primitive Vertex Already Exists");
        }
      }
      LOG_INFO("Inside non primitive block");
    }
    _adj_list[src] = std::vector<std::pair<VertexType, EdgeType>>();
    return PeakStatus::OK();
  }

  // Added method for bulk vertices insertion
  // Usage: graph.addVertices({1, 2, 3, 4, 5});
  const PeakStatus impl_addVertices(const std::vector<VertexType> &vertices) {
    PeakStatus peak_status = PeakStatus::OK();

    for (const auto &vertex : vertices) {
      if constexpr (CinderPeak::Traits::is_primitive_or_string_v<VertexType>) {
        if (auto it = _adj_list.find(vertex); it != _adj_list.end()) {
          LOG_WARNING((std::ostringstream()
                       << "Vertex " << vertex
                       << " already exists with primitive type.")
                          .str());
          peak_status = PeakStatus::VertexAlreadyExists();
          continue;
        }
        LOG_DEBUG("Unmatched vertices");
        LOG_INFO("Inside primitive block");
      } else {
        if (auto it = _adj_list.find(vertex); it != _adj_list.end()) {
          const VertexType &existingVertex = it->first;
          if (existingVertex.__id_ == vertex.__id_) {
            LOG_DEBUG("Matching vertex IDs");
            LOG_WARNING((std::ostringstream() << "Non primitive vertex "
                                              << vertex << " already exists.")
                            .str());

            peak_status = PeakStatus::VertexAlreadyExists();
            continue;
          }
        }
        LOG_INFO("Inside non primitive block");
      }

      _adj_list[vertex] = std::vector<std::pair<VertexType, EdgeType>>();
    }

    return peak_status;
  }

  // Added method for updating weight of an edge
  const PeakStatus impl_updateEdge(const VertexType &src, const VertexType &dest, 
                                  const EdgeType &newWeight) {
    if (auto it = _adj_list.find(src); it == _adj_list.end())
      return PeakStatus::VertexNotFound();
    
    // Search for the edge in source Adjacency list
    auto &edges = _adj_list.find(src)->second;
    for (auto &edge : edges) {
      if (edge.first == dest) {
        edge.second = newWeight; // Update the weight if edge exists
        return PeakStatus::OK();
      }
    }

    // If edge does not exists
    return PeakStatus::EdgeNotFound();    
  }

  bool impl_doesEdgeExist(const VertexType &src,
                          const VertexType &dest) override {
    auto it = _adj_list.find(src);
    if (it == _adj_list.end()) { // Vertex 'src' not found
      return false;
    }

    const auto &neighbors = it->second;
    for (const auto &[neighbor, edge] : neighbors) {
      if (neighbor == dest) { // Edge exists
        return true;
      }
    }
    return false;
  }

  const std::pair<EdgeType, PeakStatus>
  impl_getEdge(const VertexType &src, const VertexType &dest) override {
    auto it = _adj_list.find(src);
    if (it == _adj_list.end()) {
      return std::make_pair(EdgeType(), PeakStatus::VertexNotFound());
    }
    for (const auto &[neighbor, edge] : it->second) {
      if (neighbor == dest) {
        return std::make_pair(edge, PeakStatus::OK());
      }
    }
    return std::make_pair(EdgeType(), PeakStatus::EdgeNotFound());
  }

  const std::pair<std::vector<std::pair<VertexType, EdgeType>>, PeakStatus>
  impl_getNeighbors(const VertexType &vertex) const {
    auto it = _adj_list.find(vertex);
    if (it == _adj_list.end()) {
      static const std::vector<std::pair<VertexType, EdgeType>> empty_vec;
      return std::make_pair(empty_vec, PeakStatus::VertexNotFound());
    }
    return std::make_pair(it->second, CinderPeak::PeakStatus::OK());
  }

  auto getAdjList() { return _adj_list; }

  bool impl_doesEdgeExist(const VertexType &src, const VertexType &dest,
                          const EdgeType &weight) override {
    auto it = _adj_list.find(src);
    if (it == _adj_list.end()) {
      return false;
    }
    const auto &neighbors = it->second;
    for (const auto &[neighbor, edge] : neighbors) {
      if (neighbor == dest) {
        if (CinderPeak::Traits::isTypePrimitive<EdgeType>()) {
          LOG_CRITICAL("ID EQUAL");
        }
        return true;
      }
    }
    return false;
  }

  void print_adj_list() {
    for (const auto &[first, second] : _adj_list) {
      std::cout << "Vertex: " << first << "'s adj list:\n";
      for (const auto &pr : second) {
        std::cout << "  Neighbor: " << pr.first << " Weight: " << pr.second
                  << "\n";
      }
    }
  }
};
} // namespace PeakStore

} // namespace CinderPeak
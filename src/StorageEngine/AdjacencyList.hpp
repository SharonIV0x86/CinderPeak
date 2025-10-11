#pragma once
#include "Concepts.hpp"
#include "StorageEngine/GraphContext.hpp"
#include "StorageEngine/GraphStatistics.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <memory>
#include <shared_mutex>
namespace CinderPeak {
template <typename, typename> class PeakStorageInterface;

namespace PeakStore {

/**
 * @brief Thread safety note: This class uses a single shared mutex for
 * synchronization. Do not call these methods from within code that already
 * holds locks on this object, as it may cause deadlocks. For nested operations,
 * use the provided bulk methods instead. E.g do not try to call addEdge from
 * addEdges, it will cause a deadlock.
 */
template <typename VertexType, typename EdgeType>
class AdjacencyList
    : public CinderPeak::PeakStorageInterface<VertexType, EdgeType> {
private:
  std::unordered_map<VertexType, std::vector<std::pair<VertexType, EdgeType>>,
                     VertexHasher<VertexType>>
      _adj_list;
  mutable std::shared_mutex _mtx;

public:
  AdjacencyList() { LOG_INFO("Initialized Adjacency List object"); }

  const PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest,
                                const EdgeType &weight = EdgeType()) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    if (auto it = _adj_list.find(src); it == _adj_list.end())
      return PeakStatus::VertexNotFound();
    if (auto it = _adj_list.find(dest); it == _adj_list.end())
      return PeakStatus::VertexNotFound();
    _adj_list[src].emplace_back(dest, weight);
    return PeakStatus::OK();
  }

  template <typename EdgeContainer>
  const PeakStatus impl_addEdges(const EdgeContainer &edges) {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    PeakStatus peak_status = PeakStatus::OK();

    for (const auto &edge : edges) {
      VertexType src, dest;
      EdgeType weight = EdgeType();

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
    std::unique_lock<std::shared_mutex> lock(_mtx);

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

  const PeakStatus impl_addVertices(const std::vector<VertexType> &vertices) {
    std::unique_lock<std::shared_mutex> lock(_mtx);

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

  // Method to remove an edge
  const std::pair<EdgeType, PeakStatus>
  impl_removeEdge(const VertexType &src, const VertexType &dest) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    auto weight = EdgeType();
    if (auto it = _adj_list.find(src); it == _adj_list.end())
      return std::make_pair(weight, PeakStatus::VertexNotFound());

    auto &edges = _adj_list.find(src)->second;
    auto it = std::find_if(edges.begin(), edges.end(), [&](const auto &edge) {
      return edge.first == dest;
    });

    if (it == edges.end()) {
      return std::make_pair(weight, PeakStatus::EdgeNotFound());
    }

    weight = it->second;

    edges.erase(it);
    return std::make_pair(weight, PeakStatus::OK());
  }

  // Method to remove all vertices
  const PeakStatus impl_clearVertices() override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    _adj_list.clear();
    return PeakStatus::OK();
  }

  const PeakStatus impl_clearEdges() override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    for (auto &edge : _adj_list) {
      edge.second.clear();
    }
    return PeakStatus::OK();
  }

  const PeakStatus impl_updateEdge(const VertexType &src,
                                   const VertexType &dest,
                                   const EdgeType &newWeight) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    if (auto it = _adj_list.find(src); it == _adj_list.end())
      return PeakStatus::VertexNotFound();

    auto &edges = _adj_list.find(src)->second;
    for (auto &edge : edges) {
      if (edge.first == dest) {
        edge.second = newWeight;
        return PeakStatus::OK();
      }
    }

    return PeakStatus::EdgeNotFound();
  }

  // Method to check whether a vertex exists or not
  bool impl_hasVertex(const VertexType &v) override {
    std::shared_lock<std::shared_mutex> lock(_mtx);

    auto it = _adj_list.find(v);
    if (it == _adj_list.end()) {
      return false;
    }
    return true;
  }

  bool impl_doesEdgeExist(const VertexType &src,
                          const VertexType &dest) override {
    std::shared_lock<std::shared_mutex> lock(_mtx);

    auto it = _adj_list.find(src);
    if (it == _adj_list.end()) {

      return false;
    }

    const auto &neighbors = it->second;
    for (const auto &[neighbor, edge] : neighbors) {
      if (neighbor == dest) {
        return true;
      }
    }
    return false;
  }

  const std::pair<EdgeType, PeakStatus>
  impl_getEdge(const VertexType &src, const VertexType &dest) override {
    std::shared_lock<std::shared_mutex> lock(_mtx);

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
    std::shared_lock<std::shared_mutex> lock(_mtx);

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
    std::shared_lock<std::shared_mutex> lock(_mtx);

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
  const PeakStatus impl_removeVertex(const VertexType &v) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    // Find vertex
    auto it = _adj_list.find(v);
    if (it == _adj_list.end())
      return PeakStatus::VertexNotFound();

    _adj_list.erase(it);

    for (auto &pair : _adj_list) {
      auto &neighbors = pair.second;
      neighbors.erase(
          std::remove_if(neighbors.begin(), neighbors.end(),
                         [&](const std::pair<VertexType, EdgeType> &edge) {
                           return edge.first == v;
                         }),
          neighbors.end());
    }

    return PeakStatus::OK();
  }

  void print_adj_list() {
    std::shared_lock<std::shared_mutex> lock(_mtx);

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

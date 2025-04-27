#pragma once
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
  // TODO: combine two impl_addEdge overloads into one.
  AdjacencyList() { LOG_INFO("Initialized Adjacency List object"); }
  const PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest,
                                const EdgeType &weight) {
    if (auto it = _adj_list.find(src); it == _adj_list.end())
      return PeakStatus::VertexNotFound();
    if (auto it = _adj_list.find(dest); it == _adj_list.end())
      return PeakStatus::VertexNotFound();
    _adj_list[src].emplace_back(dest, weight);
    return PeakStatus::OK();
  }
  const PeakStatus impl_addEdge(const VertexType &src,
                                const VertexType &dest) override {
    if (auto it = _adj_list.find(src); it == _adj_list.end())
      return PeakStatus::VertexNotFound();
    if (auto it = _adj_list.find(dest); it == _adj_list.end())
      return PeakStatus::VertexNotFound();
    _adj_list[src].emplace_back(dest, EdgeType());
    return PeakStatus::OK();
  }
  const PeakStatus impl_addVertex(const VertexType &src) override {
    if constexpr (is_primitive_or_string_v<VertexType>) {
        auto it = _adj_list.find(src);
        if (it != _adj_list.end()) {
            LOG_WARNING("Vertex already exists with primitive type");
            return PeakStatus::VertexAlreadyExists("Primitive Vertex Already Exists");
        }
        LOG_DEBUG("Unmatched vertices");
        LOG_INFO("Inside primitive block");
    } else {
        auto it = _adj_list.find(src);
        if (it != _adj_list.end()) {
            const VertexType& existingVertex = it->first;
            if (existingVertex.__id_ == src.__id_) {
              LOG_DEBUG("Matching vertex IDs");
              return PeakStatus::VertexAlreadyExists("Non Primitive Vertex Already Exists");
            }
        }
        LOG_INFO("Inside non primitive block");
    }
    _adj_list[src] = std::vector<std::pair<VertexType, EdgeType>>();
    return PeakStatus::OK();
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
  const auto &getAdjList() { return _adj_list; }
  void exc() const { std::cout << "Meow\n"; }
  bool impl_doesEdgeExist(const VertexType &src, const VertexType &dest,
                          const EdgeType &weight = EdgeType()) override {
    auto it = _adj_list.find(src);
    if (it == _adj_list.end()) {
      return false;
    }
    const auto &neighbors = it->second;
    for (const auto &[neighbor, edge] : neighbors) {
      if (neighbor == dest) {
        if (isTypePrimitive<EdgeType>()) {
          LOG_CRITICAL("ID EQUAL");
        }
      }
      return true;
    }
    return false;
  }
};
} // namespace PeakStore

} // namespace CinderPeak

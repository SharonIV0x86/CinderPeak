#pragma once
#include "Utils.hpp"
namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class AdjacencyList {
private:
  std::unordered_map<VertexType, std::vector<std::pair<VertexType, EdgeType>>,
                     VertexHasher<VertexType>>
      _adj_list;

public:
  std::shared_ptr<GraphInternalMetadata> graph_metadata = nullptr;
  std::shared_ptr<GraphCreationOptions> create_options = nullptr;

  AdjacencyList(const std::shared_ptr<GraphInternalMetadata> &metadata,
                const std::shared_ptr<GraphCreationOptions> &options)
      : graph_metadata(metadata), create_options(options) {}
  // TODO: combine two impl_addEdge overloads into one.
  PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest,
                          const EdgeType &weight) {
    if (auto it = _adj_list.find(src); it == _adj_list.end())
      return PeakStatus::VertexNotFound();
    if (auto it = _adj_list.find(dest); it == _adj_list.end())
      return PeakStatus::VertexNotFound();
    _adj_list[src].emplace_back(dest, weight);
    return PeakStatus::OK();
  }
  PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest) {
    if (auto it = _adj_list.find(src); it == _adj_list.end())
      return PeakStatus::VertexNotFound();
    if (auto it = _adj_list.find(dest); it == _adj_list.end())
      return PeakStatus::VertexNotFound();
    _adj_list[src].emplace_back(dest, EdgeType());
    return PeakStatus::OK();
  }
  const PeakStatus impl_addVertex(const VertexType &src) {
    _adj_list[src] = std::vector<std::pair<VertexType, EdgeType>>();
    return PeakStatus::OK();
  }
  const std::pair<EdgeType, PeakStatus>
  impl_getEdge(const VertexType &src, const VertexType &dest) const {
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
  const std::pair<std::vector<std::pair<VertexType, EdgeType>>, PeakStatus> impl_getNeighbors(const VertexType& vertex) const {
    auto it = _adj_list.find(vertex);
    if (it == _adj_list.end()) {
        static const std::vector<std::pair<VertexType, EdgeType>> empty_vec;
        return std::make_pair(empty_vec, PeakStatus::VertexNotFound());
    }
    return std::make_pair(it->second, CinderPeak::PeakStatus::OK());
}
};
} // namespace PeakStore

} // namespace CinderPeak

#pragma once

#include "../StorageEngine/Utils.hpp"
#include <algorithm>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace CinderPeak {
namespace Algorithms {

template <typename VertexType, typename EdgeType>
using TraversalAdjacencyMap =
    std::unordered_map<VertexType, std::vector<std::pair<VertexType, EdgeType>>,
                       VertexHasher<VertexType>>;

namespace detail {

template <typename VertexType> struct TraversalVertexLess {
  bool operator()(const VertexType &a, const VertexType &b) const {
    if constexpr (has___id<VertexType>::value && std::is_class_v<VertexType> &&
                  !std::is_same_v<VertexType, std::string>) {
      return a.__id_ < b.__id_;
    } else {
      return a < b;
    }
  }
};

template <typename VertexType, typename EdgeType>
void stabilizeNeighborList(
    std::vector<std::pair<VertexType, EdgeType>> &neighbors) {
  TraversalVertexLess<VertexType> less;
  std::sort(neighbors.begin(), neighbors.end(),
            [&less](const auto &lhs, const auto &rhs) {
              return less(lhs.first, rhs.first);
            });
}

template <typename VertexType, typename EdgeType>
void finalizeSnapshotAdjacency(
    TraversalAdjacencyMap<VertexType, EdgeType> &adjacency) {
  for (auto &entry : adjacency) {
    stabilizeNeighborList<VertexType, EdgeType>(entry.second);
  }
}

} // namespace detail
} // namespace Algorithms
} // namespace CinderPeak

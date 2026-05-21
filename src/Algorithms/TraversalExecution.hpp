#pragma once

#include "Result/bfs_result.hpp"
#include "Result/dfs_result.hpp"
#include "Result/topological_sort_result.hpp"
#include "TraversalOrdering.hpp"
#include "TraversalSemantics.hpp"
#include "TraversalSnapshot.hpp"
#include <functional>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace CinderPeak {
namespace Algorithms {

/// Lightweight adjacency accessor shared by BFS and future DFS / topo sort.
template <typename VertexType, typename EdgeType>
struct TraversalAdjacencyAccess {
  using NeighborList = std::vector<std::pair<VertexType, EdgeType>>;
  using NeighborResult = std::pair<NeighborList, PeakStatus>;

  std::function<bool(const VertexType &)> hasVertex;
  std::function<NeighborResult(const VertexType &)> getNeighbors;

  [[nodiscard]] static TraversalAdjacencyAccess fromCallbacks(
      std::function<bool(const VertexType &)> has_vertex,
      std::function<NeighborResult(const VertexType &)> get_neighbors) {
    return {std::move(has_vertex), std::move(get_neighbors)};
  }

  [[nodiscard]] static TraversalAdjacencyAccess
  fromSnapshot(const TraversalSnapshot<VertexType, EdgeType> &snapshot) {
    return {
        [&snapshot](const VertexType &v) { return snapshot.hasVertex(v); },
        [&snapshot](const VertexType &v) { return snapshot.getNeighbors(v); }};
  }
};

namespace detail {

template <typename VertexType, typename EdgeType>
typename TraversalAdjacencyAccess<VertexType, EdgeType>::NeighborResult
fetchNeighbors(const TraversalAdjacencyAccess<VertexType, EdgeType> &access,
               const VertexType &vertex, TraversalNeighborOrder order) {
  using NeighborResult =
      typename TraversalAdjacencyAccess<VertexType, EdgeType>::NeighborResult;
  if (!access.getNeighbors) {
    return {typename NeighborResult::first_type{},
            PeakStatus::VertexNotFound()};
  }
  auto result = access.getNeighbors(vertex);
  if (order == TraversalNeighborOrder::DeterministicByVertex &&
      result.second.isOK()) {
    stabilizeNeighborList<VertexType, EdgeType>(result.first);
  }
  return result;
}

template <typename VertexType, typename EdgeType>
BFSResult<VertexType>
runBfs(const TraversalAdjacencyAccess<VertexType, EdgeType> &access,
       const VertexType &src, TraversalNeighborOrder neighbor_order) {
  BFSResult<VertexType> result;
  if (!access.hasVertex || !access.hasVertex(src)) {
    result._status = PeakStatus::VertexNotFound("Vertex Not Found During BFS");
    return result;
  }

  std::unordered_set<VertexType, VertexHasher<VertexType>> visited;
  std::queue<VertexType> queue;

  visited.insert(src);
  queue.push(src);

  while (!queue.empty()) {
    VertexType current = queue.front();
    queue.pop();
    result.order_.push_back(current);

    auto [neighbors, status] =
        fetchNeighbors<VertexType, EdgeType>(access, current, neighbor_order);
    if (!status.isOK()) {
      continue;
    }

    for (const auto &neighbor_pair : neighbors) {
      const VertexType &neighbor = neighbor_pair.first;
      if (visited.find(neighbor) == visited.end()) {
        visited.insert(neighbor);
        queue.push(neighbor);
      }
    }
  }

  return result;
}

template <typename VertexType, typename EdgeType>
DFSResult<VertexType>
runDfs(const TraversalAdjacencyAccess<VertexType, EdgeType> &access,
       const VertexType &src, TraversalNeighborOrder neighbor_order) {
  DFSResult<VertexType> result;
  if (!access.hasVertex || !access.hasVertex(src)) {
    result._status = PeakStatus::VertexNotFound("Vertex Not Found During DFS");
    return result;
  }

  std::unordered_set<VertexType, VertexHasher<VertexType>> visited;
  std::stack<VertexType> stack;
  stack.push(src);

  while (!stack.empty()) {
    VertexType current = stack.top();
    stack.pop();

    if (visited.find(current) != visited.end()) {
      continue;
    }
    visited.insert(current);
    result.order_.push_back(current);

    auto [neighbors, status] =
        fetchNeighbors<VertexType, EdgeType>(access, current, neighbor_order);
    if (!status.isOK()) {
      continue;
    }

    for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
      const VertexType &neighbor = it->first;
      if (visited.find(neighbor) == visited.end()) {
        stack.push(neighbor);
      }
    }
  }

  return result;
}

template <typename VertexType, typename EdgeType>
TopologicalSortResult<VertexType>
runTopologicalSort(const TraversalSnapshot<VertexType, EdgeType> &snapshot) {
  TopologicalSortResult<VertexType> result;
  const auto &adjacency = snapshot.adjacency();

  if (adjacency.empty()) {
    return result;
  }

  std::unordered_map<VertexType, std::size_t, VertexHasher<VertexType>>
      indegree;
  for (const auto &entry : adjacency) {
    indegree.emplace(entry.first, 0);
  }
  for (const auto &entry : adjacency) {
    for (const auto &edge : entry.second) {
      ++indegree[edge.first];
    }
  }

  detail::TraversalVertexLess<VertexType> less;
  std::vector<VertexType> ready;
  ready.reserve(indegree.size());
  for (const auto &entry : indegree) {
    if (entry.second == 0) {
      ready.push_back(entry.first);
    }
  }

  while (!ready.empty()) {
    std::sort(ready.begin(), ready.end(),
              [&less](const VertexType &a, const VertexType &b) {
                return less(a, b);
              });
    const VertexType current = ready.front();
    ready.erase(ready.begin());
    result.order_.push_back(current);

    auto adj_it = adjacency.find(current);
    if (adj_it == adjacency.end()) {
      continue;
    }
    for (const auto &edge : adj_it->second) {
      auto indegree_it = indegree.find(edge.first);
      if (indegree_it == indegree.end()) {
        continue;
      }
      if (indegree_it->second > 0 && --indegree_it->second == 0) {
        ready.push_back(edge.first);
      }
    }
  }

  if (result.order_.size() != adjacency.size()) {
    result.order_.clear();
    result._status = PeakStatus::GraphCycleDetected(
        "Cycle detected during topological sort");
  }

  return result;
}

} // namespace detail

template <typename VertexType, typename EdgeType>
BFSResult<VertexType>
runBfs(const TraversalAdjacencyAccess<VertexType, EdgeType> &access,
       const VertexType &src,
       TraversalNeighborOrder neighbor_order =
           TraversalNeighborOrder::PreserveBackendOrder) {
  return detail::runBfs<VertexType, EdgeType>(access, src, neighbor_order);
}

template <typename VertexType, typename EdgeType>
DFSResult<VertexType>
runDfs(const TraversalAdjacencyAccess<VertexType, EdgeType> &access,
       const VertexType &src,
       TraversalNeighborOrder neighbor_order =
           TraversalNeighborOrder::PreserveBackendOrder) {
  return detail::runDfs<VertexType, EdgeType>(access, src, neighbor_order);
}

template <typename VertexType, typename EdgeType>
TopologicalSortResult<VertexType>
runTopologicalSort(const TraversalSnapshot<VertexType, EdgeType> &snapshot) {
  return detail::runTopologicalSort<VertexType, EdgeType>(snapshot);
}

} // namespace Algorithms
} // namespace CinderPeak

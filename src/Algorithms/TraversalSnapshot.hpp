#pragma once

#include "../StorageEngine/Utils.hpp"
#include "TraversalOrdering.hpp"
#include <utility>
#include <vector>

namespace CinderPeak {
namespace Algorithms {

/// Selects which storage backend PeakStore uses when materializing a snapshot.
enum class TraversalSnapshotBackend { Adjacency, Hybrid };

/// Immutable, backend-independent adjacency view for traversal algorithms.
///
/// Ownership: returned as shared_ptr<const> from PeakStore; callers may retain
/// snapshots independently. No invalidation is performed after creation.
///
/// Immutability: adjacency is fixed at construction; neighbor lists are
/// normalized for deterministic traversal (see TraversalSemantics.hpp).
///
/// Future DFS / topological sort should consume snapshots via
/// TraversalAdjacencyAccess::fromSnapshot().
template <typename VertexType, typename EdgeType> class TraversalSnapshot {
public:
  using AdjacencyMap = TraversalAdjacencyMap<VertexType, EdgeType>;

  TraversalSnapshot() = default;

  explicit TraversalSnapshot(AdjacencyMap adjacency)
      : adjacency_(std::move(adjacency)) {
    detail::finalizeSnapshotAdjacency<VertexType, EdgeType>(adjacency_);
  }

  [[nodiscard]] bool hasVertex(const VertexType &vertex) const {
    return adjacency_.find(vertex) != adjacency_.end();
  }

  [[nodiscard]] std::pair<std::vector<std::pair<VertexType, EdgeType>>,
                          PeakStatus>
  getNeighbors(const VertexType &vertex) const {
    auto it = adjacency_.find(vertex);
    if (it == adjacency_.end()) {
      return {std::vector<std::pair<VertexType, EdgeType>>{},
              PeakStatus::VertexNotFound()};
    }
    return {it->second, PeakStatus::OK()};
  }

  [[nodiscard]] std::size_t vertexCount() const { return adjacency_.size(); }

  [[nodiscard]] const AdjacencyMap &adjacency() const { return adjacency_; }

private:
  AdjacencyMap adjacency_;
};

} // namespace Algorithms
} // namespace CinderPeak

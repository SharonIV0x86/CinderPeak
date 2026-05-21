#pragma once

#include "../StorageEngine/Utils.hpp"
#include <unordered_map>
#include <utility>
#include <vector>

namespace CinderPeak {
namespace Algorithms {

/// Selects which storage backend PeakStore uses when materializing a snapshot.
enum class TraversalSnapshotBackend { Adjacency, Hybrid };

/// Immutable, backend-independent adjacency view for traversal algorithms.
///
/// Snapshots are owned by the caller (typically via shared_ptr from PeakStore)
/// and capture graph connectivity at creation time. Live storage mutations do
/// not affect an existing snapshot.
///
/// Future algorithms (DFS, topological sort, concurrent policies) should prefer
/// this view over direct storage access.
template <typename VertexType, typename EdgeType> class TraversalSnapshot {
public:
  using AdjacencyMap =
      std::unordered_map<VertexType,
                         std::vector<std::pair<VertexType, EdgeType>>,
                         VertexHasher<VertexType>>;

  TraversalSnapshot() = default;

  explicit TraversalSnapshot(AdjacencyMap adjacency)
      : adjacency_(std::move(adjacency)) {}

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

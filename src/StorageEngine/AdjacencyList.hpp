#pragma once
#include "Concepts.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/GraphContext.hpp"
#include "StorageEngine/GraphStatistics.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <atomic>
#include <memory>
#include <optional>
#include <shared_mutex>

namespace CinderPeak {
template <typename, typename> class PeakStorageInterface;

namespace PeakStore {

/**
 * @brief ID-based AdjacencyList storage engine.
 *
 * Public API methods keep the same signatures as your previous implementation
 * (impl_addVertex, impl_addEdge, impl_getNeighbors, etc.) but internally the
 * structure uses engine-owned VertexId (uint64_t) for adjacency and fast
 * traversal.
 *
 * Thread-safety: single shared mutex used for simplicity (same contract as
 * before). Methods return PeakStatus or (EdgeType, PeakStatus) to maintain
 * compatibility.
 */
template <typename VertexType, typename EdgeType>
class AdjacencyList
    : public CinderPeak::PeakStorageInterface<VertexType, EdgeType> {
private:
  // Internal adjacency keyed by VertexId for fast traversal & compact storage
  std::unordered_map<CinderPeak::VertexId,
                     std::vector<std::pair<CinderPeak::VertexId, EdgeType>>>
      _adj;

  // Engine maps to maintain user objects and lookups
  std::unordered_map<CinderPeak::VertexId, VertexType> _vertex_data;
  std::unordered_map<VertexType, CinderPeak::VertexId, VertexHasher<VertexType>>
      _vertex_lookup;

  // Next engine vertex id
  std::atomic<CinderPeak::VertexId> _next_vertex_id{1};

  mutable std::shared_mutex _mtx;
  PolicyHandler pHandler;

  // INTERNAL helper: get VertexId for a given VertexType; returns optional
  std::optional<CinderPeak::VertexId>
  lookupVertexId_nolock(const VertexType &v) const {
    auto it = _vertex_lookup.find(v);
    if (it == _vertex_lookup.end())
      return std::nullopt;
    return it->second;
  }

  // INTERNAL helper: ensure existence and return id or set PeakStatus
  std::optional<CinderPeak::VertexId>
  ensureVertexExists_nolock(const VertexType &v, PeakStatus &out_status) const {
    auto id_opt = lookupVertexId_nolock(v);
    if (!id_opt) {
      out_status = PeakStatus::VertexNotFound();
      return std::nullopt;
    }
    out_status = PeakStatus::OK();
    return id_opt;
  }

public:
  // Keep constructor signature compatible
  AdjacencyList(const PolicyHandler &pHandler) : pHandler(pHandler) {
    // reserve small default capacity to reduce rehashing
    _adj.reserve(1024);
    _vertex_data.reserve(1024);
    _vertex_lookup.reserve(1024);
  }
  AdjacencyList() : pHandler() {
    _adj.reserve(1024);
    _vertex_data.reserve(1024);
    _vertex_lookup.reserve(1024);
  }

  // Add a single vertex. Public API accepts VertexType and engine assigns a
  // VertexId.
  const PeakStatus impl_addVertex(const VertexType &v) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    if (auto it = _vertex_lookup.find(v); it != _vertex_lookup.end()) {
      // Preserve legacy semantics expected by tests
      if constexpr (CinderPeak::Traits::is_primitive_or_string_v<VertexType>) {
        return PeakStatus::VertexAlreadyExists(
            "Primitive Vertex Already Exists");
      } else {
        return PeakStatus::VertexAlreadyExists(
            "Non Primitive Vertex Already Exists");
      }
    }

    const VertexId id = _next_vertex_id.fetch_add(1, std::memory_order_relaxed);

    _vertex_lookup.emplace(v, id);
    _vertex_data.emplace(id, v);
    _adj.emplace(id, std::vector<std::pair<VertexId, EdgeType>>());

    return PeakStatus::OK();
  }

  // Add multiple vertices in bulk
  const PeakStatus impl_addVertices(const std::vector<VertexType> &vertices) {
    std::unique_lock<std::shared_mutex> lock(_mtx);
    PeakStatus final_status = PeakStatus::OK();

    for (const auto &v : vertices) {
      if (_vertex_lookup.find(v) != _vertex_lookup.end()) {
        final_status = PeakStatus::VertexAlreadyExists();
        continue;
      }
      VertexId id = _next_vertex_id.fetch_add(1, std::memory_order_relaxed);
      _vertex_lookup.emplace(v, id);
      _vertex_data.emplace(id, v);
      _adj.emplace(id, std::vector<std::pair<VertexId, EdgeType>>());
    }
    return final_status;
  }

  // Add a single edge (by user-visible VertexType). Preserves directed
  // semantics.
  const PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest,
                                const EdgeType &weight = EdgeType()) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    auto srcIt = _vertex_lookup.find(src);
    if (srcIt == _vertex_lookup.end())
      return PeakStatus::VertexNotFound();
    auto destIt = _vertex_lookup.find(dest);
    if (destIt == _vertex_lookup.end())
      return PeakStatus::VertexNotFound();

    VertexId srcId = srcIt->second;
    VertexId destId = destIt->second;

    // Append neighbor; duplicate edges allowed only if policy allows
    // if (!pHandler.cfg()) {
    //   auto &neighbors = _adj[srcId];
    //   for (const auto &pr : neighbors) {
    //     if (pr.first == destId) {
    //       return PeakStatus::EdgeAlreadyExists();
    //     }
    //   }
    // }

    _adj[srcId].emplace_back(destId, weight);
    return PeakStatus::OK();
  }

  // Bulk add edges. EdgeContainer can be vector<pair<VertexType, VertexType>>
  // or vector<tuple<VertexType, VertexType, EdgeType>>
  template <typename EdgeContainer>
  const PeakStatus impl_addEdges(const EdgeContainer &edges) {
    std::unique_lock<std::shared_mutex> lock(_mtx);
    PeakStatus overall = PeakStatus::OK();

    for (const auto &edge : edges) {
      VertexType src;
      VertexType dest;
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
      } else {
        // Unsupported container element type
        overall = PeakStatus::Unimplemented();
        continue;
      }

      auto srcIt = _vertex_lookup.find(src);
      if (srcIt == _vertex_lookup.end()) {
        LOG_WARNING(
            (std::ostringstream() << "The vertex does not exist (src)").str());
        overall = PeakStatus::VertexNotFound();
        continue;
      }
      auto destIt = _vertex_lookup.find(dest);
      if (destIt == _vertex_lookup.end()) {
        LOG_WARNING(
            (std::ostringstream() << "The vertex does not exist (dest)").str());
        overall = PeakStatus::VertexNotFound();
        continue;
      }

      VertexId srcId = srcIt->second;
      VertexId destId = destIt->second;

      // if (!pHandler.allowParallelEdges()) {
      //   auto &neighbors = _adj[srcId];
      //   bool exists = false;
      //   for (const auto &pr : neighbors) {
      //     if (pr.first == destId) {
      //       exists = true;
      //       break;
      //     }
      //   }
      //   if (exists) {
      //     overall = PeakStatus::EdgeAlreadyExists();
      //     continue;
      //   }
      // }

      _adj[srcId].emplace_back(destId, weight);
    }

    return overall;
  }

  // Remove an edge (returns old weight and status)
  const std::pair<EdgeType, PeakStatus>
  impl_removeEdge(const VertexType &src, const VertexType &dest) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);
    EdgeType retWeight = EdgeType();

    auto srcIt = _vertex_lookup.find(src);
    if (srcIt == _vertex_lookup.end())
      return std::make_pair(retWeight, PeakStatus::VertexNotFound());
    auto destIt = _vertex_lookup.find(dest);
    if (destIt == _vertex_lookup.end())
      return std::make_pair(retWeight, PeakStatus::VertexNotFound());

    VertexId srcId = srcIt->second;
    VertexId destId = destIt->second;

    auto &neighbors = _adj[srcId];
    auto it = std::find_if(neighbors.begin(), neighbors.end(),
                           [&](const auto &p) { return p.first == destId; });

    if (it == neighbors.end())
      return std::make_pair(retWeight, PeakStatus::EdgeNotFound());

    retWeight = it->second;
    neighbors.erase(it);
    return std::make_pair(retWeight, PeakStatus::OK());
  }

  // Update an existing edge's weight
  const PeakStatus impl_updateEdge(const VertexType &src,
                                   const VertexType &dest,
                                   const EdgeType &newWeight) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    auto srcIt = _vertex_lookup.find(src);
    if (srcIt == _vertex_lookup.end())
      return PeakStatus::VertexNotFound();
    auto destIt = _vertex_lookup.find(dest);
    if (destIt == _vertex_lookup.end())
      return PeakStatus::VertexNotFound();

    VertexId srcId = srcIt->second;
    VertexId destId = destIt->second;

    auto &neighbors = _adj[srcId];
    for (auto &p : neighbors) {
      if (p.first == destId) {
        p.second = newWeight;
        return PeakStatus::OK();
      }
    }
    return PeakStatus::EdgeNotFound();
  }

  // Check if a vertex exists (by VertexType)
  bool impl_hasVertex(const VertexType &v) override {
    std::shared_lock<std::shared_mutex> lock(_mtx);
    return _vertex_lookup.find(v) != _vertex_lookup.end();
  }

  // Check if edge exists (src->dest) ignoring weight
  bool impl_doesEdgeExist(const VertexType &src,
                          const VertexType &dest) override {
    std::shared_lock<std::shared_mutex> lock(_mtx);

    auto srcIt = _vertex_lookup.find(src);
    if (srcIt == _vertex_lookup.end())
      return false;
    auto destIt = _vertex_lookup.find(dest);
    if (destIt == _vertex_lookup.end())
      return false;

    VertexId srcId = srcIt->second;
    VertexId destId = destIt->second;

    const auto &neighbors = _adj.at(srcId);
    for (const auto &p : neighbors) {
      if (p.first == destId)
        return true;
    }
    return false;
  }

  // Check if edge exists with an exact weight match
  bool impl_doesEdgeExist(const VertexType &src, const VertexType &dest,
                          const EdgeType &weight) override {
    std::shared_lock<std::shared_mutex> lock(_mtx);

    auto srcIt = _vertex_lookup.find(src);
    if (srcIt == _vertex_lookup.end())
      return false;
    auto destIt = _vertex_lookup.find(dest);
    if (destIt == _vertex_lookup.end())
      return false;

    VertexId srcId = srcIt->second;
    VertexId destId = destIt->second;

    const auto &neighbors = _adj.at(srcId);
    for (const auto &p : neighbors) {
      if (p.first == destId && p.second == weight)
        return true;
    }
    return false;
  }

  // Get edge weight (if exists)
  const std::pair<EdgeType, PeakStatus>
  impl_getEdge(const VertexType &src, const VertexType &dest) override {
    std::shared_lock<std::shared_mutex> lock(_mtx);

    auto srcIt = _vertex_lookup.find(src);
    if (srcIt == _vertex_lookup.end())
      return std::make_pair(EdgeType(), PeakStatus::VertexNotFound());
    auto destIt = _vertex_lookup.find(dest);
    if (destIt == _vertex_lookup.end())
      return std::make_pair(EdgeType(), PeakStatus::VertexNotFound());

    VertexId srcId = srcIt->second;
    VertexId destId = destIt->second;

    const auto &neighbors = _adj.at(srcId);
    for (const auto &p : neighbors) {
      if (p.first == destId)
        return std::make_pair(p.second, PeakStatus::OK());
    }
    return std::make_pair(EdgeType(), PeakStatus::EdgeNotFound());
  }

  // Get neighbors in the same public signature as before:
  // vector<pair<VertexType, EdgeType>> This will build the list by translating
  // VertexId => VertexType via _vertex_data.
  const std::pair<std::vector<std::pair<VertexType, EdgeType>>, PeakStatus>
  impl_getNeighbors(const VertexType &vertex) const {
    std::shared_lock<std::shared_mutex> lock(_mtx);

    auto it = _vertex_lookup.find(vertex);
    if (it == _vertex_lookup.end()) {
      static const std::vector<std::pair<VertexType, EdgeType>> empty_vec;
      return std::make_pair(empty_vec, PeakStatus::VertexNotFound());
    }

    VertexId id = it->second;
    const auto &neighbors = _adj.at(id);

    std::vector<std::pair<VertexType, EdgeType>> result;
    result.reserve(neighbors.size());
    for (const auto &p : neighbors) {
      // _vertex_data must contain the target id; defensive check
      auto vdataIt = _vertex_data.find(p.first);
      if (vdataIt != _vertex_data.end()) {
        result.emplace_back(vdataIt->second, p.second);
      } else {
        // Should not happen in consistent state; skip silently
      }
    }
    return std::make_pair(result, PeakStatus::OK());
  }

  // Remove a vertex (and all incident edges). Works by id internally.
  const PeakStatus impl_removeVertex(const VertexType &v) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    auto it = _vertex_lookup.find(v);
    if (it == _vertex_lookup.end())
      return PeakStatus::VertexNotFound();

    VertexId id = it->second;

    // Erase outgoing edges
    _adj.erase(id);

    // Erase incoming edges (scan adjacency lists)
    for (auto &pair : _adj) {
      auto &neighbors = pair.second;
      neighbors.erase(
          std::remove_if(neighbors.begin(), neighbors.end(),
                         [&](const std::pair<VertexId, EdgeType> &edge) {
                           return edge.first == id;
                         }),
          neighbors.end());
    }

    // Remove vertex maps
    _vertex_lookup.erase(it);
    _vertex_data.erase(id);

    return PeakStatus::OK();
  }

  // Clear all vertices and edges
  const PeakStatus impl_clearVertices() override {
    std::unique_lock<std::shared_mutex> lock(_mtx);
    _adj.clear();
    _vertex_lookup.clear();
    _vertex_data.clear();
    _next_vertex_id.store(1, std::memory_order_relaxed);
    return PeakStatus::OK();
  }

  const PeakStatus impl_clearEdges() override {
    std::unique_lock<std::shared_mutex> lock(_mtx);
    for (auto &pair : _adj) {
      pair.second.clear();
    }
    return PeakStatus::OK();
  }

  // Debug printer (prints vertex data via stored VertexType)
  void print_adj_list() {
    std::shared_lock<std::shared_mutex> lock(_mtx);
    for (const auto &kv : _adj) {
      VertexId id = kv.first;
      auto vdIt = _vertex_data.find(id);
      if (vdIt == _vertex_data.end())
        continue;
      const VertexType &v = vdIt->second;
      std::cout << "Vertex (id=" << id
                << "): " << /* try to print user type */ "" << "\n";
      for (const auto &pr : kv.second) {
        VertexId nbId = pr.first;
        auto nbIt = _vertex_data.find(nbId);
        if (nbIt != _vertex_data.end()) {
          std::cout << "  Neighbor id=" << nbId << "\n";
        } else {
          std::cout << "  Neighbor id=" << nbId << " (no data)\n";
        }
      }
    }
  }

  //
  // Internal accessors for GraphEngine / CSR builder:
  // Return const references so higher-level orchestrator can read adjacency
  // and vertex data without copying. These are internal helpers; avoid
  // exposing them broadly in public API.
  //
  const std::unordered_map<
      CinderPeak::VertexId,
      std::vector<std::pair<CinderPeak::VertexId, EdgeType>>> &
  getInternalAdjacency() const {
    return _adj;
  }

  const std::unordered_map<CinderPeak::VertexId, VertexType> &
  getVertexDataMap() const {
    return _vertex_data;
  }
};

} // namespace PeakStore

} // namespace CinderPeak

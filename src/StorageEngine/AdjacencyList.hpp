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

template <typename VertexType, typename EdgeType>
class AdjacencyList
    : public CinderPeak::PeakStorageInterface<VertexType, EdgeType> {
private:
  std::unordered_map<CinderPeak::VertexId,
                     std::vector<std::pair<CinderPeak::VertexId, EdgeType>>>
      _adj;
  std::unordered_map<CinderPeak::VertexId, VertexType> _vertex_data;
  std::unordered_map<VertexType, CinderPeak::VertexId, VertexHasher<VertexType>>
      _vertex_lookup;

  std::atomic<CinderPeak::VertexId> _next_vertex_id{1};

  mutable std::shared_mutex _mtx;
  const PolicyHandler pHandler;

  std::optional<CinderPeak::VertexId>
  lookupVertexId_nolock(const VertexType &v) const {
    auto it = _vertex_lookup.find(v);
    if (it == _vertex_lookup.end())
      return std::nullopt;
    return it->second;
  }

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
  AdjacencyList(const PolicyHandler &handler) : pHandler(handler) {
    _adj.reserve(1024);
    _vertex_data.reserve(1024);
    _vertex_lookup.reserve(1024);
  }

  const PeakStatus impl_addVertex(const VertexType &v) override {
    std::string logMsg;
    VertexId assignedId = 0;
    {
      std::unique_lock<std::shared_mutex> lock(_mtx);

      if (auto it = _vertex_lookup.find(v); it != _vertex_lookup.end()) {
        if constexpr (CinderPeak::Traits::is_primitive_or_string_v<
                          VertexType>) {
          return PeakStatus::VertexAlreadyExists(
              "Primitive Vertex Already Exists");
        } else {
          return PeakStatus::VertexAlreadyExists(
              "Non Primitive Vertex Already Exists");
        }
      }

      assignedId = _next_vertex_id.fetch_add(1, std::memory_order_relaxed);

      _vertex_lookup.try_emplace(v, assignedId);
      _vertex_data.try_emplace(assignedId, v);
      _adj.try_emplace(assignedId);

      logMsg =
          std::string("Vertex added with id=") + std::to_string(assignedId);
    }

    // perform logging outside of the lock to avoid blocking critical sections
    pHandler.log(LogLevel::INFO, logMsg);
    return PeakStatus::OK();
  }

  const PeakStatus impl_addVertices(const std::vector<VertexType> &vertices) {
    std::unique_lock<std::shared_mutex> lock(_mtx);
    PeakStatus final_status = PeakStatus::OK();

    for (const auto &v : vertices) {
      if (_vertex_lookup.find(v) != _vertex_lookup.end()) {
        final_status = PeakStatus::VertexAlreadyExists();
        continue;
      }
      VertexId id = _next_vertex_id.fetch_add(1, std::memory_order_relaxed);
      _vertex_lookup.try_emplace(v, id);
      _vertex_data.try_emplace(id, v);
      _adj.try_emplace(id);
    }
    return final_status;
  }

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

    // Append neighbor; duplicate edges allowed only if policy allows, this is
    controlled by PeakStore auto &neighbors = _adj[srcId];
    neighbors.emplace_back(destId, weight);
    return PeakStatus::OK();
  }

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

      _adj[srcId].emplace_back(destId, weight);
    }

    return overall;
  }

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

  bool impl_hasVertex(const VertexType &v) override {
    std::shared_lock<std::shared_mutex> lock(_mtx);
    return _vertex_lookup.find(v) != _vertex_lookup.end();
  }

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
      auto vdataIt = _vertex_data.find(p.first);
      if (vdataIt != _vertex_data.end()) {
        result.emplace_back(vdataIt->second, p.second);
      }
    }
    return std::make_pair(result, PeakStatus::OK());
  }

  const PeakStatus impl_removeVertex(const VertexType &v) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    auto it = _vertex_lookup.find(v);
    if (it == _vertex_lookup.end())
      return PeakStatus::VertexNotFound();

    VertexId id = it->second;

    _adj.erase(id);

    for (auto &pair : _adj) {
      auto &neighbors = pair.second;
      neighbors.erase(
          std::remove_if(neighbors.begin(), neighbors.end(),
                         [&](const std::pair<VertexId, EdgeType> &edge) {
                           return edge.first == id;
                         }),
          neighbors.end());
    }

    _vertex_lookup.erase(it);
    _vertex_data.erase(id);

    return PeakStatus::OK();
  }

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

  void print_adj_list() {
    std::shared_lock<std::shared_mutex> lock(_mtx);
    for (const auto &kv : _adj) {
      VertexId id = kv.first;
      auto vdIt = _vertex_data.find(id);
      if (vdIt == _vertex_data.end())
        continue;
      const VertexType &v = vdIt->second;
      std::cout << "Vertex (id=" << id << "): " << "\n";
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

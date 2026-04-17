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
  AdjacencyList() : {
    _adj.reserve(1024);
    _vertex_data.reserve(1024);
    _vertex_lookup.reserve(1024);
  }

  [[nodiscard]] const PeakStatus impl_addVertex(const VertexType &v) override {
    pHandler.log(LogLevel::DEBUG, "Executing impl_addVertex");
    VertexId assignedId = 0;
    {
      std::unique_lock<std::shared_mutex> lock(_mtx);

      if (auto it = _vertex_lookup.find(v); it != _vertex_lookup.end()) {
        if constexpr (CinderPeak::Traits::is_primitive_or_string_v<
                          VertexType>) {
          pHandler.log(LogLevel::WARNING,
                       "Failed to add Vertex: Vertex Already Exist.");
          return PeakStatus::VertexAlreadyExists(
              "Primitive Vertex Already Exists");
        } else {
          pHandler.log(LogLevel::WARNING,
                       "Failed to add Non Premitive Vertex: Non Premitive "
                       "Vertex Already Exist.");
          return PeakStatus::VertexAlreadyExists(
              "Non Primitive Vertex Already Exists");
        }
      }

      assignedId = _next_vertex_id.fetch_add(1, std::memory_order_relaxed);

      _vertex_lookup.try_emplace(v, assignedId);
      _vertex_data.try_emplace(assignedId, v);
      _adj.try_emplace(assignedId);
    }

    // perform string construction and logging outside of the lock to avoid
    // blocking critical sections
    // TODO: this is a test log for output check so remove it in future.
    pHandler.log(LogLevel::INFO, "Vertex added.");
    return PeakStatus::OK();
  }

  [[nodiscard]] const PeakStatus
  impl_addVertices(const std::vector<VertexType> &vertices) {
    pHandler.log(LogLevel::DEBUG, "Executing impl_addVertices");
    std::unique_lock<std::shared_mutex> lock(_mtx);
    PeakStatus final_status = PeakStatus::OK();

    for (const auto &v : vertices) {
      if (_vertex_lookup.find(v) != _vertex_lookup.end()) {
        final_status = PeakStatus::VertexAlreadyExists();
        pHandler.log(LogLevel::WARNING, "Vertex already Exist.");
        continue;
      }
      VertexId id = _next_vertex_id.fetch_add(1, std::memory_order_relaxed);
      _vertex_lookup.try_emplace(v, id);
      _vertex_data.try_emplace(id, v);
      _adj.try_emplace(id);
    }
    pHandler.log(LogLevel::INFO, "Vertex Added successfully.");

    return final_status;
  }

  [[nodiscard]] const PeakStatus
  impl_addEdge(const VertexType &src, const VertexType &dest,
               const EdgeType &weight = EdgeType()) override {
    pHandler.log(LogLevel::DEBUG, "Executing impl_addEdge");
    std::unique_lock<std::shared_mutex> lock(_mtx);

    auto srcIt = _vertex_lookup.find(src);
    if (srcIt == _vertex_lookup.end()) {
      return PeakStatus::VertexNotFound();
    }

    auto destIt = _vertex_lookup.find(dest);
    if (destIt == _vertex_lookup.end()) {
      pHandler.log(LogLevel::WARNING, "Vertex not found.");
      return PeakStatus::VertexNotFound();
    }

    VertexId srcId = srcIt->second;
    VertexId destId = destIt->second;

    // Append neighbor; duplicate edges allowed only if policy allows.
    auto &neighbors = _adj[srcId];
    neighbors.emplace_back(destId, weight);

    pHandler.log(LogLevel::INFO, "Edge successfully added between vertices.");

    return PeakStatus::OK();
  }

  template <typename EdgeContainer>
  [[nodiscard]] const PeakStatus impl_addEdges(const EdgeContainer &edges) {
    pHandler.log(LogLevel::DEBUG, "Executing impl_addEdges");
    std::vector<std::string> warnings;
    PeakStatus overall = PeakStatus::OK();

    {
      std::unique_lock<std::shared_mutex> lock(_mtx);

      for (const auto &edge : edges) {
        VertexType src;
        VertexType dest;
        EdgeType weight = EdgeType();

        if constexpr (std::is_same_v<typename EdgeContainer::value_type,
                                     std::pair<VertexType, VertexType>>) {
          src = edge.first;
          dest = edge.second;
        } else if constexpr (std::is_same_v<typename EdgeContainer::value_type,
                                            std::tuple<VertexType, VertexType,
                                                       EdgeType>>) {
          src = std::get<0>(edge);
          dest = std::get<1>(edge);
          weight = std::get<2>(edge);
        } else {
          overall = PeakStatus::Unimplemented();
          continue;
        }

        auto srcIt = _vertex_lookup.find(src);
        if (srcIt == _vertex_lookup.end()) {
          warnings.push_back("The vertex does not exist (src)");
          overall = PeakStatus::VertexNotFound();
          continue;
        }
        auto destIt = _vertex_lookup.find(dest);
        if (destIt == _vertex_lookup.end()) {
          warnings.push_back("The vertex does not exist (dest)");
          overall = PeakStatus::VertexNotFound();
          continue;
        }

        VertexId srcId = srcIt->second;
        VertexId destId = destIt->second;

        _adj[srcId].emplace_back(destId, weight);
      }
    }
    pHandler.log(LogLevel::INFO, "Multiple edges processed successfully.");

    return overall;
  }

  [[nodiscard]] const std::pair<EdgeType, PeakStatus>
  impl_removeEdge(const VertexType &src, const VertexType &dest) override {
    pHandler.log(LogLevel::DEBUG, "Executing impl_removeEdge");
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

    if (it == neighbors.end()) {
      pHandler.log(LogLevel::WARNING, "Edge not found.");
      return std::make_pair(retWeight, PeakStatus::EdgeNotFound());
    }

    retWeight = it->second;
    neighbors.erase(it);
    pHandler.log(LogLevel::INFO, "Edge successfully removed between vertices.");

    return std::make_pair(retWeight, PeakStatus::OK());
  }

  [[nodiscard]] const PeakStatus
  impl_updateEdge(const VertexType &src, const VertexType &dest,
                  const EdgeType &newWeight) override {
    pHandler.log(LogLevel::DEBUG, "Executing impl_updateEdge");
    std::unique_lock<std::shared_mutex> lock(_mtx);

    auto srcIt = _vertex_lookup.find(src);
    if (srcIt == _vertex_lookup.end()) {
      pHandler.log(LogLevel::WARNING, "Vertex not found.");
      return PeakStatus::VertexNotFound();
    }
    auto destIt = _vertex_lookup.find(dest);
    if (destIt == _vertex_lookup.end()) {
      pHandler.log(LogLevel::WARNING, "Vertex not found.");
      return PeakStatus::VertexNotFound();
    }

    VertexId srcId = srcIt->second;
    VertexId destId = destIt->second;

    auto &neighbors = _adj[srcId];
    for (auto &p : neighbors) {
      if (p.first == destId) {
        p.second = newWeight;
        return PeakStatus::OK();
      }
    }
    pHandler.log(LogLevel::INFO, "Edge Not Found.");
    return PeakStatus::EdgeNotFound();
  }

  [[nodiscard]] bool impl_hasVertex(const VertexType &v) noexcept override {
    pHandler.log(LogLevel::DEBUG, "Executing impl_hasVertex");
    std::shared_lock<std::shared_mutex> lock(_mtx);
    pHandler.log(LogLevel::INFO, "Vertex lookup.");

    return _vertex_lookup.find(v) != _vertex_lookup.end();
  }

  [[nodiscard]] bool
  impl_doesEdgeExist(const VertexType &src,
                     const VertexType &dest) noexcept override {
    pHandler.log(LogLevel::DEBUG, "Executing impl_doesEdgeExist");
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
    pHandler.log(LogLevel::INFO, "Edge found.");
    return false;
  }

  [[nodiscard]] bool
  impl_doesEdgeExist(const VertexType &src, const VertexType &dest,
                     const EdgeType &weight) noexcept override {
    pHandler.log(LogLevel::DEBUG, "Executing impl_doesEdgeExist");
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
      if (p.first == destId && p.second == weight) {
        pHandler.log(LogLevel::INFO, "Edge not exist.");
        return true;
      }
    }
    pHandler.log(LogLevel::INFO, "Edge not exist.");

    return false;
  }

  [[nodiscard]] const std::pair<EdgeType, PeakStatus>
  impl_getEdge(const VertexType &src, const VertexType &dest) override {
    pHandler.log(LogLevel::DEBUG, "Executing impl_getEdge");
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
    pHandler.log(LogLevel::INFO, "Edge not found");
    return std::make_pair(EdgeType(), PeakStatus::EdgeNotFound());
  }

  const std::pair<std::vector<std::pair<VertexType, EdgeType>>, PeakStatus>
  impl_getNeighbors(const VertexType &vertex) const {
    pHandler.log(LogLevel::DEBUG, "Executing impl_getNeighbors");
    // data copied under lock
    std::vector<std::pair<VertexId, EdgeType>> neighbor_ids;
    std::unordered_map<VertexId, VertexType> vertex_data_snapshot;

    {
      std::shared_lock<std::shared_mutex> lock(_mtx);

      auto it = _vertex_lookup.find(vertex);
      if (it == _vertex_lookup.end()) {
        return std::make_pair(std::vector<std::pair<VertexType, EdgeType>>{},
                              PeakStatus::VertexNotFound());
      }

      VertexId id = it->second;
      const auto &neighbors = _adj.at(id);

      // copy neighbor list
      neighbor_ids = neighbors;

      // copy relevant vertex data for neighbors
      for (const auto &p : neighbor_ids) {
        auto vdataIt = _vertex_data.find(p.first);
        if (vdataIt != _vertex_data.end()) {
          vertex_data_snapshot.try_emplace(vdataIt->first, vdataIt->second);
        }
      }
    }

    // build result outside of lock
    std::vector<std::pair<VertexType, EdgeType>> result;
    result.reserve(neighbor_ids.size());
    for (const auto &p : neighbor_ids) {
      auto vdataIt = vertex_data_snapshot.find(p.first);
      if (vdataIt != vertex_data_snapshot.end()) {
        result.emplace_back(vdataIt->second, p.second);
      }
    }
    pHandler.log(LogLevel::INFO, "Edge successfully added between vertices.");
    return std::make_pair(result, PeakStatus::OK());
  }

  [[nodiscard]] const PeakStatus
  impl_removeVertex(const VertexType &v) override {
    pHandler.log(LogLevel::DEBUG, "Executing impl_removeVertex");
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
    pHandler.log(LogLevel::INFO, "Vertex successfully removed.");

    return PeakStatus::OK();
  }

  [[nodiscard]] const PeakStatus impl_clearVertices() override {
    pHandler.log(LogLevel::DEBUG, "Executing impl_clearVertices");
    std::unique_lock<std::shared_mutex> lock(_mtx);
    _adj.clear();
    _vertex_lookup.clear();
    _vertex_data.clear();
    _next_vertex_id.store(1, std::memory_order_relaxed);
    pHandler.log(LogLevel::INFO, "Vertex successfully Cleared.");

    return PeakStatus::OK();
  }

  [[nodiscard]] const PeakStatus impl_clearEdges() override {
    pHandler.log(LogLevel::DEBUG, "Executing impl_clearEdges");
    std::unique_lock<std::shared_mutex> lock(_mtx);
    for (auto &pair : _adj) {
      pair.second.clear();
    }
    pHandler.log(LogLevel::INFO, "cleared all Edges from Graph.");

    return PeakStatus::OK();
  }

  void print_adj_list() {
    // data copied under lock
    struct VertexInfo {
      VertexId id;
      VertexType vertex_data;
      std::vector<std::pair<VertexId, EdgeType>> neighbor_ids;
    };
    std::vector<VertexInfo> vertices_to_print;

    {
      std::shared_lock<std::shared_mutex> lock(_mtx);
      for (const auto &kv : _adj) {
        VertexId id = kv.first;
        auto vdIt = _vertex_data.find(id);
        if (vdIt == _vertex_data.end())
          continue;

        VertexInfo info;
        info.id = id;
        info.vertex_data = vdIt->second;
        info.neighbor_ids = kv.second;

        vertices_to_print.push_back(info);
      }
    }

    // perform all I/O outside of lock
    for (const auto &vertex_info : vertices_to_print) {
      std::cout << "Vertex (id=" << vertex_info.id << "): " << "\n";
      for (const auto &neighbor_pair : vertex_info.neighbor_ids) {
        VertexId nbId = neighbor_pair.first;
        std::cout << "  Neighbor id=" << nbId << "\n";
      }
    }
  }

  const std::unordered_map<
      CinderPeak::VertexId,
      std::vector<std::pair<CinderPeak::VertexId, EdgeType>>> &
  getInternalAdjacency() const {
    pHandler.log(LogLevel::DEBUG, "Executing getInternalAdjacency");
    return _adj;
  }

  std::string impl_toDot(bool isDirected, bool allowParallel) const {
    pHandler.log(LogLevel::DEBUG, "Executing impl_toDot");
    std::shared_lock<std::shared_mutex> lock(_mtx);
    std::stringstream ss;

    if (!allowParallel) {
      ss << "strict ";
    }
    ss << (isDirected ? "digraph" : "graph") << " G {\n";
    ss << "  rankdir=LR;\n";
    ss << "  node[shape=circle style=filled fillcolor=\"#E3F2FD\" "
          "fontname=\"Arial\"];\n";
    ss << "  edge[fontname=\"Arial\" fontsize=10];\n\n";

    // declare all nodes first (ensures isolated nodes appear)
    for (const auto &kv : _vertex_data) {
      VertexId id = kv.first;
      const VertexType &v = kv.second;

      ss << "  node_" << id << " [label=\"";
      ss << v;
      ss << "\"];\n";
    }

    // draw Edges
    std::string connector = isDirected ? "->" : "--";
    for (const auto &kv : _adj) {
      VertexId srcId = kv.first;
      const auto &neighbors = kv.second;

      for (const auto &edge : neighbors) {
        VertexId destId = edge.first;
        const EdgeType &weight = edge.second;

        ss << "  node_" << srcId << " " << connector << " node_" << destId;

        if constexpr (!Traits::is_unweighted_v<EdgeType>) {
          ss << " [label=\"" << weight << "\"]";
        }
        ss << ";\n";
      }
    }
    ss << "}\n";
    return ss.str();
  }

  const std::unordered_map<CinderPeak::VertexId, VertexType> &
  getVertexDataMap() const {
    pHandler.log(LogLevel::DEBUG, "Executing getVertexDataMap");
    return _vertex_data;
  }
};

} // namespace PeakStore

} // namespace CinderPeak
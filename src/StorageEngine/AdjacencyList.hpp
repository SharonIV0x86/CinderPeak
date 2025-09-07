#pragma once
#include "StorageEngine/ErrorCodes.hpp"
#include <unordered_map>
#include <vector>
#include <utility>

namespace CinderPeak {

template <typename VertexType, typename EdgeType>
class AdjacencyList {
private:
    // Assuming adjacency_map stores vertices -> (neighbor, weight)
    std::unordered_map<VertexType, std::vector<std::pair<VertexType, EdgeType>>> adjacency_map;

public:
    AdjacencyList() = default;

    PeakStatus impl_addVertex(const VertexType &v) {
        if (adjacency_map.find(v) == adjacency_map.end()) {
            adjacency_map[v] = {};
            return PeakStatus::OK();
        }
        return PeakStatus::VertexAlreadyExists();
    }

    PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest, const EdgeType &weight) {
        adjacency_map[src].push_back({dest, weight});
        return PeakStatus::OK();
    }


    PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest) {
        adjacency_map[src].push_back({dest, EdgeType()});
        return PeakStatus::OK();
        
    return peak_status;
  }

  // Method for updating weight of an edge
  const PeakStatus impl_updateEdge(const VertexType &src, const VertexType &dest, 
                                  const EdgeType &newWeight) {
    if (auto it = _adj_list.find(src); it == _adj_list.end())
      return PeakStatus::VertexNotFound();
    
    // Search for the edge in source Adjacency list
    auto &edges = _adj_list.find(src)->second;
    for (auto &edge : edges) {
      if (edge.first == dest) {
        edge.second = newWeight; // Update the weight if edge exists
        return PeakStatus::OK();
      }
    }

    // If edge does not exists
    return PeakStatus::EdgeNotFound();    
  }

  bool impl_doesEdgeExist(const VertexType &src,
                          const VertexType &dest) override {
    auto it = _adj_list.find(src);
    if (it == _adj_list.end()) { // Vertex 'src' not found
      return false;

    }

    bool impl_doesEdgeExist(const VertexType &src, const VertexType &dest) {
        auto it = adjacency_map.find(src);
        if (it == adjacency_map.end()) return false;
        for (auto &edge : it->second) {
            if (edge.first == dest) return true;
        }
        return false;
    }

    bool impl_doesEdgeExist(const VertexType &src, const VertexType &dest, const EdgeType &weight) {
        auto it = adjacency_map.find(src);
        if (it == adjacency_map.end()) return false;
        for (auto &edge : it->second) {
            if (edge.first == dest && edge.second == weight) return true;
        }
        return false;
    }

    std::pair<EdgeType, PeakStatus> impl_getEdge(const VertexType &src, const VertexType &dest) {
        auto it = adjacency_map.find(src);
        if (it == adjacency_map.end()) {
            return {EdgeType(), PeakStatus::VertexNotFound()};
        }
        for (auto &edge : it->second) {
            if (edge.first == dest) return {edge.second, PeakStatus::OK()};
        }
        return {EdgeType(), PeakStatus::EdgeNotFound()};
    }

    std::pair<std::vector<std::pair<VertexType, EdgeType>>, PeakStatus>
    impl_getNeighbors(const VertexType &src) const {
        auto it = adjacency_map.find(src);
        if (it == adjacency_map.end()) {
            return {{}, PeakStatus::VertexNotFound()};
        }
        return {it->second, PeakStatus::OK()};
    }

    // ✅ NEW METHOD
    PeakStatus impl_clearEdges() {
        for (auto &entry : adjacency_map) {
            entry.second.clear();  // remove all outgoing edges
        }
        return PeakStatus::OK();
    }
};

} // namespace CinderPeak

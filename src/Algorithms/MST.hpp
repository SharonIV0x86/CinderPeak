#pragma once
#include "../Concepts.hpp"
#include "../StorageInterface.hpp"
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace CinderPeak::Algorithms {

template <typename VertexType, typename EdgeType>
struct MSTEdge {
    VertexType u;
    VertexType v;
    EdgeType weight;

    bool operator>(const MSTEdge& other) const {
        return weight > other.weight;
    }
};

template <typename VertexType, typename EdgeType>
std::vector<MSTEdge<VertexType, EdgeType>> primMST(const PeakStorageInterface<VertexType, EdgeType>& storage) {
    STATIC_ASSERT_WEIGHTED(EdgeType);
    STATIC_ASSERT_NUMERIC_EDGE(EdgeType);

    std::vector<MSTEdge<VertexType, EdgeType>> mst;
    auto vertices = storage.impl_getAllVertices();
    if (vertices.empty()) return mst;

    std::unordered_set<VertexType, VertexHasher<VertexType>> visited;
    std::priority_queue<MSTEdge<VertexType, EdgeType>, std::vector<MSTEdge<VertexType, EdgeType>>, std::greater<MSTEdge<VertexType, EdgeType>>> pq;

    VertexType startNode = vertices[0];
    visited.insert(startNode);

    auto addEdges = [&](const VertexType& u) {
        auto [neighbors, status] = storage.impl_getNeighbors(u);
        if (!status.isOK()) return;

        for (const auto& edge : neighbors) {
            if (visited.find(edge.first) == visited.end()) {
                pq.push({u, edge.first, edge.second});
            }
        }
    };

    addEdges(startNode);

    while (!pq.empty() && mst.size() < vertices.size() - 1) {
        MSTEdge<VertexType, EdgeType> edge = pq.top();
        pq.pop();

        if (visited.find(edge.v) != visited.end()) continue;

        visited.insert(edge.v);
        mst.push_back(edge);
        addEdges(edge.v);
    }

    return mst;
}

} // namespace CinderPeak::Algorithms

#pragma once
#include "../CinderGraph.hpp"
#include <queue>
#include <unordered_map>
#include <vector>
#include <limits>
#include <algorithm>

namespace CinderPeak::Algorithms {

template <typename VertexType, typename EdgeType>
struct DijkstraResult {
    std::unordered_map<VertexType, EdgeType, VertexHasher<VertexType>> distances;
    std::unordered_map<VertexType, VertexType, VertexHasher<VertexType>> predecessors;
};

template <typename VertexType, typename EdgeType>
DijkstraResult<VertexType, EdgeType> dijkstra(const CinderGraph<VertexType, EdgeType>& graph, const VertexType& startVertex) {
    STATIC_ASSERT_WEIGHTED(EdgeType);
    STATIC_ASSERT_NUMERIC_EDGE(EdgeType);

    using DistanceMap = std::unordered_map<VertexType, EdgeType, VertexHasher<VertexType>>;
    using PredecessorMap = std::unordered_map<VertexType, VertexType, VertexHasher<VertexType>>;
    
    DistanceMap distances;
    PredecessorMap predecessors;
    
    auto vertices = graph.getAllVertices();
    for (const auto& v : vertices) {
        distances[v] = std::numeric_limits<EdgeType>::max();
    }

    if (distances.find(startVertex) == distances.end()) {
        return {distances, predecessors};
    }

    distances[startVertex] = 0;

    using PQElement = std::pair<EdgeType, VertexType>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;

    pq.push({0, startVertex});

    while (!pq.empty()) {
        EdgeType d = pq.top().first;
        VertexType u = pq.top().second;
        pq.pop();

        if (d > distances[u]) continue;

        auto neighbors = graph.getNeighbors(u);
        for (const auto& edge : neighbors) {
            VertexType v = edge.first;
            EdgeType weight = edge.second;

            if (distances[u] + weight < distances[v]) {
                distances[v] = distances[u] + weight;
                predecessors[v] = u;
                pq.push({distances[v], v});
            }
        }
    }

    return {distances, predecessors};
}

template <typename VertexType, typename EdgeType>
struct BellmanFordResult {
    std::unordered_map<VertexType, EdgeType, VertexHasher<VertexType>> distances;
    std::unordered_map<VertexType, VertexType, VertexHasher<VertexType>> predecessors;
    bool hasNegativeCycle;
};

template <typename VertexType, typename EdgeType>
BellmanFordResult<VertexType, EdgeType> bellmanFord(const CinderGraph<VertexType, EdgeType>& graph, const VertexType& startVertex) {
    STATIC_ASSERT_WEIGHTED(EdgeType);
    STATIC_ASSERT_NUMERIC_EDGE(EdgeType);

    auto vertices = graph.getAllVertices();
    std::unordered_map<VertexType, EdgeType, VertexHasher<VertexType>> distances;
    std::unordered_map<VertexType, VertexType, VertexHasher<VertexType>> predecessors;

    for (const auto& v : vertices) {
        distances[v] = std::numeric_limits<EdgeType>::max();
    }

    if (distances.find(startVertex) == distances.end()) {
        return {distances, predecessors, false};
    }

    distances[startVertex] = 0;

    for (size_t i = 1; i < vertices.size(); ++i) {
        bool changed = false;
        for (const auto& u : vertices) {
            if (distances[u] == std::numeric_limits<EdgeType>::max()) continue;
            auto neighbors = graph.getNeighbors(u);
            for (const auto& edge : neighbors) {
                if (distances[u] + edge.second < distances[edge.first]) {
                    distances[edge.first] = distances[u] + edge.second;
                    predecessors[edge.first] = u;
                    changed = true;
                }
            }
        }
        if (!changed) break;
    }

    // Check for negative cycles
    for (const auto& u : vertices) {
        if (distances[u] == std::numeric_limits<EdgeType>::max()) continue;
        auto neighbors = graph.getNeighbors(u);
        for (const auto& edge : neighbors) {
            if (distances[u] + edge.second < distances[edge.first]) {
                return {distances, predecessors, true};
            }
        }
    }

    return {distances, predecessors, false};
}

template <typename VertexType, typename EdgeType>
std::vector<VertexType> reconstructPath(const std::unordered_map<VertexType, VertexType, VertexHasher<VertexType>>& predecessors, 
                                       const VertexType& target) {
    std::vector<VertexType> path;
    auto it = predecessors.find(target);
    if (it == predecessors.end()) {
        // Check if target is the start (no predecessor but might be in distances)
        // This function needs more context or target must be in predecessors.
        // Usually, if target has no predecessor and isn't the start, it's unreachable.
        return path;
    }

    VertexType current = target;
    path.push_back(current);
    while (predecessors.count(current)) {
        current = predecessors.at(current);
        path.push_back(current);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

} // namespace CinderPeak::Algorithms

#pragma once
#include "../CinderGraph.hpp"
#include <vector>
#include <unordered_map>
#include <queue>

namespace CinderPeak::Algorithms {

template <typename VertexType, typename EdgeType>
std::vector<VertexType> topologicalSort(const CinderGraph<VertexType, EdgeType>& graph) {
    std::unordered_map<VertexType, int, VertexHasher<VertexType>> inDegree;
    auto vertices = graph.getAllVertices();

    for (const auto& v : vertices) {
        inDegree[v] = 0;
    }

    for (const auto& u : vertices) {
        auto neighbors = graph.getNeighbors(u);
        for (const auto& edge : neighbors) {
            inDegree[edge.first]++;
        }
    }

    std::queue<VertexType> q;
    for (const auto& v : vertices) {
        if (inDegree[v] == 0) {
            q.push(v);
        }
    }

    std::vector<VertexType> result;
    while (!q.empty()) {
        VertexType u = q.front();
        q.pop();
        result.push_back(u);

        auto neighbors = graph.getNeighbors(u);
        for (const auto& edge : neighbors) {
            VertexType v = edge.first;
            inDegree[v]--;
            if (inDegree[v] == 0) {
                q.push(v);
            }
        }
    }

    if (result.size() != vertices.size()) {
        // Cycle detected or graph is not a DAG
        return {};
    }

    return result;
}

template <typename VertexType, typename EdgeType>
bool hasCycle(const CinderGraph<VertexType, EdgeType>& graph) {
    auto sorted = topologicalSort(graph);
    auto vertices = graph.getAllVertices();
    if (vertices.empty()) return false;
    return sorted.size() != vertices.size();
}

} // namespace CinderPeak::Algorithms

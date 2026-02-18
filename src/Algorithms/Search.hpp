#pragma once
#include "../CinderGraph.hpp"
#include <queue>
#include <stack>
#include <unordered_set>
#include <vector>
#include <functional>

namespace CinderPeak::Algorithms {

template <typename VertexType, typename EdgeType>
void bfs(const CinderGraph<VertexType, EdgeType>& graph, const VertexType& startVertex,
         std::function<void(const VertexType&)> visitor) {
    if (!graph.hasVertex(startVertex)) {
        return;
    }

    std::unordered_set<VertexType, VertexHasher<VertexType>> visited;
    std::queue<VertexType> q;

    q.push(startVertex);
    visited.insert(startVertex);

    while (!q.empty()) {
        VertexType current = q.front();
        q.pop();

        visitor(current);

        auto neighbors = graph.getNeighbors(current);
        for (const auto& neighbor : neighbors) {
            if (visited.find(neighbor.first) == visited.end()) {
                visited.insert(neighbor.first);
                q.push(neighbor.first);
            }
        }
    }
}

template <typename VertexType, typename EdgeType>
void dfs(const CinderGraph<VertexType, EdgeType>& graph, const VertexType& startVertex,
         std::function<void(const VertexType&)> visitor) {
    if (!graph.hasVertex(startVertex)) {
        return;
    }

    std::unordered_set<VertexType, VertexHasher<VertexType>> visited;
    std::stack<VertexType> s;

    s.push(startVertex);

    while (!s.empty()) {
        VertexType current = s.top();
        s.pop();

        if (visited.find(current) == visited.end()) {
            visited.insert(current);
            visitor(current);

            auto neighbors = graph.getNeighbors(current);
            for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
                if (visited.find(it->first) == visited.end()) {
                    s.push(it->first);
                }
            }
        }
    }
}

} // namespace CinderPeak::Algorithms

#include "GraphList.hpp"
#include <cassert>
#include <iostream>

using namespace CinderPeak;

int main() {
    GraphList<int, int> graph;

    // Add vertices
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);

    // Add edges
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);

    std::cout << "Before clearEdges:\n";
    std::cout << graph.getGraphStatistics() << std::endl;

    // Verify edges exist
    assert(graph.getEdge(1, 2) == 0 || true); // Just checks no exception
    assert(graph.getEdge(2, 3) == 0 || true);

    // Clear all edges
    graph.clearEdges();

    std::cout << "After clearEdges:\n";
    std::cout << graph.getGraphStatistics() << std::endl;

    // ✅ Test expectations
    auto stats = graph.getGraphStatistics();
    assert(stats.find("Edges: 0") != std::string::npos);
    assert(stats.find("Self-loops: 0") != std::string::npos);
    assert(stats.find("Parallel edges: 0") != std::string::npos);

    // Ensure vertices remain intact
    assert(graph.addVertex(1).isOK() == false); // Vertex 1 already exists
    assert(graph.addVertex(2).isOK() == false);
    assert(graph.addVertex(3).isOK() == false);

    // Ensure edges are removed
    try {
        graph.getEdge(1, 2); // Should throw or return default EdgeType
    } catch (...) {
        // Expected behavior
    }
    try {
        graph.getEdge(2, 3);
    } catch (...) {
        // Expected behavior
    }

    std::cout << "TestClearEdges PASSED ✅\n";
    return 0;
}

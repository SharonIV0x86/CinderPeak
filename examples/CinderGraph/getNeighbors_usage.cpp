#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
  try {
    // Weighted directed graph
    cout << "--- Weighted Directed Graph ---" << endl;
    CinderGraph<int, int> g;
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);
    g.addVertex(4);

    g.addEdge(1, 2, 10);
    g.addEdge(1, 3, 20);
    g.addEdge(1, 4, 30);
    g.addEdge(2, 3, 5);

    auto neighbors = g.getNeighbors(1);
    cout << "Neighbors of vertex 1:" << endl;
    for (const auto &[vertex, weight] : neighbors) {
      cout << "  -> " << vertex << " (weight: " << weight << ")" << endl;
    }

    // Vertex with no outgoing edges
    auto neighbors2 = g.getNeighbors(4);
    if (neighbors2.empty()) {
      cout << "Neighbors of vertex 4: none" << endl;
    }

    // Non-existent vertex returns empty
    auto neighbors3 = g.getNeighbors(99);
    if (neighbors3.empty()) {
      cout << "Neighbors of vertex 99 (not in graph): empty" << endl;
    }

    // Weighted undirected graph
    cout << "\n--- Weighted Undirected Graph ---" << endl;
    GraphCreationOptions undirectedOpts({GraphCreationOptions::Undirected});
    CinderGraph<int, double> g2(undirectedOpts);
    g2.addVertex(1);
    g2.addVertex(2);
    g2.addVertex(3);

    g2.addEdge(1, 2, 1.5);
    g2.addEdge(2, 3, 2.5);

    // In an undirected graph both endpoints see each other as a neighbor
    auto n1 = g2.getNeighbors(2);
    cout << "Neighbors of vertex 2:" << endl;
    for (const auto &[vertex, weight] : n1) {
      cout << "  -> " << vertex << " (weight: " << weight << ")" << endl;
    }

    // String vertices
    cout << "\n--- String Vertices ---" << endl;
    CinderGraph<string, int> g3;
    g3.addVertex("A");
    g3.addVertex("B");
    g3.addVertex("C");

    g3.addEdge("A", "B", 42);
    g3.addEdge("A", "C", 100);

    auto sn = g3.getNeighbors("A");
    cout << "Neighbors of vertex A:" << endl;
    for (const auto &[vertex, weight] : sn) {
      cout << "  -> " << vertex << " (weight: " << weight << ")" << endl;
    }

    return 0;
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }
}

#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
  try {
    // 1. Basic vertex addition
    cout << "--- Scenario 1: Basic Usage ---" << endl;
    GraphMatrix<int, double> g1;
    auto [v1, added1] = g1.addVertex(1);
    cout << "Added vertex " << v1 << ": " << (added1 ? "success" : "failed")
         << endl;

    auto [v2, added2] = g1.addVertex(2);
    auto [v3, added3] = g1.addVertex(3);
    cout << "Added vertices 2 and 3" << endl;

    // 2. Edge case: Duplicate vertex
    cout << "\n--- Scenario 2: Duplicate Vertex ---" << endl;
    auto [v4, added4] = g1.addVertex(1);
    cout << "Adding duplicate vertex 1: " << (added4 ? "success" : "failed")
         << endl;

    // 3. String vertices with different graph configurations
    cout << "\n--- Scenario 3: Directed Weighted Graph ---" << endl;
    GraphCreationOptions directedOpts({GraphCreationOptions::Directed});
    GraphMatrix<string, float> g2(directedOpts);

    auto [vs1, addedS1] = g2.addVertex("NodeA");
    auto [vs2, addedS2] = g2.addVertex("NodeB");
    auto [vs3, addedS3] = g2.addVertex("NodeC");
    cout << "Added 3 string vertices to directed graph" << endl;

    // 4. Undirected graph with self-loops
    cout << "\n--- Scenario 4: Undirected Graph with Self-loops ---" << endl;
    GraphCreationOptions undirectedOpts(
        {GraphCreationOptions::Undirected, GraphCreationOptions::SelfLoops});
    GraphMatrix<int, int> g3(undirectedOpts);

    for (int i = 0; i < 5; i++) {
      auto [v, added] = g3.addVertex(i * 10);
      cout << "Added vertex " << v << ": " << (added ? "success" : "failed")
           << endl;
    }

    // 5. Combined with immediate edge operations
    cout << "\n--- Scenario 5: Add Vertex then Edge ---" << endl;
    GraphMatrix<int, double> g4;
    auto [va, addedA] = g4.addVertex(100);
    auto [vb, addedB] = g4.addVertex(200);

    if (addedA && addedB) {
      auto [edge, edgeAdded] = g4.addEdge(100, 200, 5.5);
      cout << "After adding vertices, added edge: "
           << (edgeAdded ? "success" : "failed") << endl;
    }

    return 0;
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }
}

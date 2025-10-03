#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
  try {
    // 1. Basic weighted edge addition
    cout << "--- Scenario 1: Basic Weighted Edges ---" << endl;
    GraphMatrix<int, double> g1;
    g1.addVertex(1);
    g1.addVertex(2);
    g1.addVertex(3);

    auto [edge1, added1] = g1.addEdge(1, 2, 5.5);
    cout << "Added edge (1,2) with weight 5.5: "
         << (added1 ? "success" : "failed") << endl;

    auto [edge2, added2] = g1.addEdge(2, 3, 10.75);
    cout << "Added edge (2,3) with weight 10.75: "
         << (added2 ? "success" : "failed") << endl;

    // 2. Edge case: Vertices don't exist
    cout << "\n--- Scenario 2: Invalid Vertex References ---" << endl;
    GraphMatrix<int, float> g2;
    g2.addVertex(10);

    auto [edge3, added3] = g2.addEdge(10, 99, 3.14f);
    cout << "Adding edge (10,99) where 99 doesn't exist: "
         << (added3 ? "success" : "failed") << endl;

    auto [edge4, added4] = g2.addEdge(100, 200, 2.71f);
    cout << "Adding edge (100,200) where neither exists: "
         << (added4 ? "success" : "failed") << endl;

    // 3. Directed vs Undirected
    cout << "\n--- Scenario 3: Directed Graph ---" << endl;
    GraphCreationOptions dirOpts({GraphCreationOptions::Directed});
    GraphMatrix<string, int> g3(dirOpts);

    g3.addVertex("A");
    g3.addVertex("B");
    g3.addVertex("C");

    g3.addEdge("A", "B", 100);
    g3.addEdge("B", "C", 200);
    cout << "Added directed edges A->B and B->C" << endl;

    cout << "\n--- Scenario 3b: Undirected Graph ---" << endl;
    GraphCreationOptions undirOpts({GraphCreationOptions::Undirected});
    GraphMatrix<string, int> g4(undirOpts);

    g4.addVertex("X");
    g4.addVertex("Y");
    g4.addEdge("X", "Y", 50);
    cout << "Added undirected edge X-Y (creates edge in both directions)"
         << endl;

    // 4. Parallel edges configuration
    cout << "\n--- Scenario 4: Parallel Edges ---" << endl;
    GraphCreationOptions parallelOpts(
        {GraphCreationOptions::Directed, GraphCreationOptions::ParallelEdges});
    GraphMatrix<int, int> g5(parallelOpts);

    g5.addVertex(1);
    g5.addVertex(2);

    g5.addEdge(1, 2, 100);
    cout << "Added first edge (1,2) with weight 100" << endl;

    g5.addEdge(1, 2, 200);
    cout << "Added parallel edge (1,2) with weight 200" << endl;

    // 5. Combined with matrix operator
    cout << "\n--- Scenario 5: Matrix Operator vs addEdge ---" << endl;
    GraphMatrix<int, double> g6;
    g6.addVertex(5);
    g6.addVertex(6);
    g6.addVertex(7);

    // Using addEdge
    auto [e5, a5] = g6.addEdge(5, 6, 1.5);
    cout << "Added edge (5,6) using addEdge: " << (a5 ? "success" : "failed")
         << endl;

    // Using operator[]
    g6[6][7] = 2.5;
    cout << "Added edge (6,7) using operator[]: 2.5" << endl;

    return 0;
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }
}

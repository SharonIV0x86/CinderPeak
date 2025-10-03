#include "CinderPeak.hpp"
#include <iostream>
#include <string>

using namespace CinderPeak;
using namespace std;

int main() {
  try {
    // 1. Basic statistics - Empty graph
    cout << "=== Scenario 1: Empty Graph Statistics ===" << endl;
    GraphMatrix<int, double> g1;
    cout << g1.getGraphStatistics() << endl;

    // 2. Statistics after adding vertices only
    cout << "=== Scenario 2: Graph with Vertices Only ===" << endl;
    GraphMatrix<int, int> g2;
    g2.addVertex(1);
    g2.addVertex(2);
    g2.addVertex(3);
    g2.addVertex(4);
    cout << g2.getGraphStatistics() << endl;

    // 3. Statistics with edges - Undirected graph
    cout << "=== Scenario 3: Undirected Graph with Edges ===" << endl;
    GraphCreationOptions undirectedOpts({GraphCreationOptions::Undirected});
    GraphMatrix<string, double> g3(undirectedOpts);

    g3.addVertex("A");
    g3.addVertex("B");
    g3.addVertex("C");
    g3.addVertex("D");

    g3.addEdge("A", "B", 1.5);
    g3.addEdge("B", "C", 2.5);
    g3.addEdge("C", "D", 3.5);
    g3.addEdge("A", "D", 4.5);

    cout << g3.getGraphStatistics() << endl;

    // 4. Statistics with self-loops
    cout << "=== Scenario 4: Graph with Self-loops ===" << endl;
    GraphCreationOptions selfLoopOpts(
        {GraphCreationOptions::Undirected, GraphCreationOptions::SelfLoops});
    GraphMatrix<int, int> g4(selfLoopOpts);

    g4.addVertex(10);
    g4.addVertex(20);
    g4.addVertex(30);

    g4.addEdge(10, 20, 100);
    g4.addEdge(20, 30, 200);
    g4.addEdge(10, 10, 50); // Self-loop
    g4.addEdge(20, 20, 75); // Self-loop

    cout << g4.getGraphStatistics() << endl;

    // 5. Dense graph - Directed
    cout << "=== Scenario 5: Dense Directed Graph ===" << endl;
    GraphCreationOptions directedOpts({GraphCreationOptions::Directed});
    GraphMatrix<int, float> g5(directedOpts);

    // Add vertices
    for (int i = 1; i <= 5; i++) {
      g5.addVertex(i);
    }

    // Add many edges to create a dense graph
    g5.addEdge(1, 2, 1.0f);
    g5.addEdge(1, 3, 1.5f);
    g5.addEdge(1, 4, 2.0f);
    g5.addEdge(2, 3, 2.5f);
    g5.addEdge(2, 4, 3.0f);
    g5.addEdge(2, 5, 3.5f);
    g5.addEdge(3, 4, 4.0f);
    g5.addEdge(3, 5, 4.5f);
    g5.addEdge(4, 5, 5.0f);

    cout << g5.getGraphStatistics() << endl;

    // 6. Sparse graph
    cout << "=== Scenario 6: Sparse Graph ===" << endl;
    GraphMatrix<string, int> g6;

    g6.addVertex("Node1");
    g6.addVertex("Node2");
    g6.addVertex("Node3");
    g6.addVertex("Node4");
    g6.addVertex("Node5");
    g6.addVertex("Node6");
    g6.addVertex("Node7");
    g6.addVertex("Node8");

    // Add only a few edges
    g6.addEdge("Node1", "Node2", 10);
    g6.addEdge("Node3", "Node5", 20);

    cout << g6.getGraphStatistics() << endl;

    // 7. Unweighted graph statistics
    cout << "=== Scenario 7: Unweighted Graph ===" << endl;
    GraphMatrix<int, Unweighted> g7;

    g7.addVertex(100);
    g7.addVertex(200);
    g7.addVertex(300);

    g7.addEdge(100, 200);
    g7.addEdge(200, 300);

    cout << g7.getGraphStatistics() << endl;

    // 8. After removing vertices/edges
    cout << "=== Scenario 8: Statistics After Modifications ===" << endl;
    GraphMatrix<int, double> g8;

    g8.addVertex(1);
    g8.addVertex(2);
    g8.addVertex(3);
    g8.addVertex(4);

    g8.addEdge(1, 2, 1.0);
    g8.addEdge(2, 3, 2.0);
    g8.addEdge(3, 4, 3.0);

    cout << "Before removal:" << endl;
    cout << g8.getGraphStatistics() << endl;

    // Remove a vertex
    g8.removeVertex(3);

    cout << "After removing vertex 3:" << endl;
    cout << g8.getGraphStatistics() << endl;

    return 0;
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }
}

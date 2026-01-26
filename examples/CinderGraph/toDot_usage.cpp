#include "CinderPeak.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace CinderPeak;
using namespace std;

int main() {
  try {
    PolicyConfiguration policy(PolicyConfiguration::ErrorPolicy::Throw,
                               PolicyConfiguration::LoggingPolicy::LogConsole);

    // ===== 1. Directed + Weighted Graph with Cycles =====
    cout << "\n--- Directed Weighted Graph (Cycles) ---" << endl;

    GraphCreationOptions directedOpts({GraphCreationOptions::Directed});
    CinderGraph<int, int> g1(directedOpts, policy);

    for (int v : {1, 2, 3, 4}) {
      g1.addVertex(v);
    }

    g1.addEdge(1, 2, 100);
    g1.addEdge(2, 3, 200);
    g1.addEdge(3, 4, 300);
    g1.addEdge(4, 1, 400); // cycle
    g1.addEdge(1, 3, 500); // cross edge

    cout << "Vertices: " << g1.numVertices() << endl;
    cout << "Edges:" << g1.numEdges() << endl;

    // ===== 2. Isolated Nodes =====
    cout << "\n--- Graph with Isolated Nodes ---" << endl;

    CinderGraph<int, int> g2(directedOpts, policy);
    g2.addVertex(10);
    g2.addVertex(20);
    g2.addVertex(30); // isolated
    g2.addEdge(10, 20, 210);

    cout << "Vertices " << g2.numVertices() << endl;
    cout << "Edges " << g2.numEdges() << endl;

    // ===== 3. Parallel Edges (Directed) =====
    cout << "\n--- Parallel Edges ---" << endl;

    GraphCreationOptions parallelOpts(
        {GraphCreationOptions::Directed, GraphCreationOptions::ParallelEdges});

    CinderGraph<int, int> g3(parallelOpts, policy);
    g3.addVertex(1);
    g3.addVertex(2);

    g3.addEdge(1, 2, 100);
    g3.addEdge(1, 2, 200);

    cout << "Parallel edges count: " << g3.numEdges() << endl;

    // ===== 4. Undirected Graph =====
    cout << "\n--- Undirected Graph ---" << endl;

    CinderGraph<int, int> g4;
    g4.addVertex(1);
    g4.addVertex(2);
    g4.addVertex(3);

    g4.addEdge(1, 2, 100);
    g4.addEdge(2, 3, 20);

    cout << "Vertices: " << g4.numVertices() << endl;
    cout << "Edges: " << g4.numEdges() << endl;

    // ===== 5. Non-numeric Vertices =====
    cout << "\n--- String Vertices ---" << endl;

    CinderGraph<string, float> g5;
    g5.addVertex("Delhi");
    g5.addVertex("Mumbai");
    g5.addVertex("Kolkata");

    g5.addEdge("Delhi", "Mumbai", 1400.0f);
    g5.addEdge("Delhi", "Kolkata", 1500.0f);

    cout << "Node graph edges: " << g5.numEdges() << endl;

    // ===== 6. Traversal Vertification =====
    cout << "\n--- BFS Traversal (g1 from 1) ---" << endl;

    auto bfsResult = g1.bfs(1);
    if (bfsResult.isOK()) {
      for (const auto &v : bfsResult.order_) {
        cout << v << "->";
      }

      cout << "END" << endl;
    }

    // ===== 7. DOT Export Verification =====
    cout << "\n--- DOT Export (Directed Graph) ---" << endl;
    g1.toDot("g1_directed.dot");
    cout << "Exported to g1_directed.dot" << endl;

    cout << "\n--- DOT Export (Isolated Nodes) ---" << endl;
    g2.toDot("g2_isolated.dot");
    cout << "Exported to g2_isolated.dot" << endl;

    cout << "\n --- DOT Export (Parallel Edges) ---" << endl;
    g3.toDot("g3_parallel.dot");
    cout << "Exported to g3_parallel.dot" << endl;

    cout << "\n --- DOT Export (Undirected Graph) ---" << endl;
    g4.toDot("g4_undirected.dot");
    cout << "Exported to g4_undirected.dot" << endl;

    cout << "\n --- DOT Export (String Vertices) ---" << endl;
    g5.toDot("g5_string.dot");
    cout << "Exported to g5_string.dot" << endl;

    // ===== 8. File Export Verification =====
    cout << "\n--- DOT File Export ---" << endl;
    string filename = "graph_output.dot";
    g5.toDot(filename);
    cout << "Exported g1 to file: " << filename << endl;

    return 0;
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }
}
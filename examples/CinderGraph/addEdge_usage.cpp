#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
  try {
    // 1. Unweighted graph
    cout << "--- Unweighted Graph ---" << endl;
    CinderGraph<int, Unweighted> g1;
    g1.addVertex(1);
    g1.addVertex(2);
    g1.addVertex(3);

    auto [edge1, added1] = g1.addEdge(1, 2);
    cout << "Added unweighted edge (1,2): " << (added1 ? "success" : "failed")
         << endl;

    auto [edge2, added2] = g1.addEdge(2, 3);
    cout << "Added unweighted edge (2,3): " << (added2 ? "success" : "failed")
         << endl;
    cout << "Total edges: " << g1.numEdges() << endl;

    // 2. Weighted graph
    cout << "\n--- Weighted Graph ---" << endl;
    CinderGraph<int, double> g2;
    g2.addVertex(10);
    g2.addVertex(20);
    g2.addVertex(30);

    auto [edge3, added3] = g2.addEdge(10, 20, 5.5);
    cout << "Added weighted edge (10,20) with weight 5.5: "
         << (added3 ? "success" : "failed") << endl;

    auto [edge4, added4] = g2.addEdge(20, 30, 12.75);
    cout << "Added weighted edge (20,30) with weight 12.75: "
         << (added4 ? "success" : "failed") << endl;
    cout << "Total edges: " << g2.numEdges() << endl;

    // 3. Graph with parallel edges
    cout << "\n--- Parallel Edges ---" << endl;
    GraphCreationOptions parallelOpts(
        {GraphCreationOptions::Directed, GraphCreationOptions::ParallelEdges});
    CinderGraph<int, int> g3(parallelOpts);
    g3.addVertex(1);
    g3.addVertex(2);

    g3.addEdge(1, 2, 100);
    cout << "Added edge (1,2) with weight 100" << endl;

    g3.addEdge(1, 2, 200);
    cout << "Added parallel edge (1,2) with weight 200" << endl;
    cout << "Total edges: " << g3.numEdges() << endl;

    // 4. Error handling - vertices don't exist
    cout << "\n--- Error Cases ---" << endl;
    CinderGraph<int, Unweighted> g4;
    auto [edge5, added5] = g4.addEdge(100, 200);
    cout << "Adding edge without vertices: " << (added5 ? "success" : "failed")
         << endl;

    // 5. String vertices with weighted edges
    cout << "\n--- String Vertices ---" << endl;
    CinderGraph<string, float> g5;
    g5.addVertex("New York");
    g5.addVertex("Los Angeles");
    g5.addVertex("Chicago");

    g5.addEdge("New York", "Los Angeles", 2451.0f);
    g5.addEdge("New York", "Chicago", 790.0f);
    cout << "Added city distance edges, total: " << g5.numEdges() << endl;

    return 0;
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }
}
